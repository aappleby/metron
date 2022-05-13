#include "MtModLibrary.h"

#include <sys/stat.h>

#include "Log.h"
#include "MtField.h"
#include "MtFuncParam.h"
#include "MtMethod.h"
#include "MtModule.h"
#include "MtSourceFile.h"
#include "MtTracer.h"
#include "metron_tools.h"

#pragma warning(disable : 4996)

std::vector<std::string> split_field_path(const std::string &path);

//------------------------------------------------------------------------------

MtModule *MtModLibrary::get_module(const std::string &name) {
  for (auto mod : modules) {
    if (mod->mod_name == name) return mod;
  }
  return nullptr;
}

MtSourceFile *MtModLibrary::get_source(const std::string &name) {
  for (auto s : source_files) {
    if (s->filename == name) return s;
  }
  return nullptr;
}

//------------------------------------------------------------------------------

void MtModLibrary::teardown() {
  modules.clear();
  for (auto s : source_files) delete s;
}

//------------------------------------------------------------------------------

void MtModLibrary::add_search_path(const std::string &path) {
  search_paths.push_back(path);
}

//------------------------------------------------------------------------------

void MtModLibrary::add_source(MtSourceFile *source_file) {
  source_file->lib = this;
  source_files.push_back(source_file);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModLibrary::load_source(const char *filename,
                                           MtSourceFile *&out_source,
                                           bool verbose) {
  Err err;

  if (get_source(filename)) {
    return WARN("Duplicate filename %s\n", filename);
  }

  bool found = false;
  for (auto &path : search_paths) {
    auto full_path = path.size() ? path + "/" + filename : filename;
    struct stat s;
    auto stat_result = stat(full_path.c_str(), &s);
    if (stat_result == 0) {
      found = true;
      if (verbose) LOG_B("Loading %s from %s\n", filename, full_path.c_str());
      LOG_INDENT_SCOPE();

      std::string src_blob;
      src_blob.resize(s.st_size);

      auto f = fopen(full_path.c_str(), "rb");
      fread((void *)src_blob.data(), 1, src_blob.size(), f);
      fclose(f);

      bool use_utf8_bom = false;
      if (uint8_t(src_blob[0]) == 239 && uint8_t(src_blob[1]) == 187 &&
          uint8_t(src_blob[2]) == 191) {
        use_utf8_bom = true;
        src_blob.erase(src_blob.begin(), src_blob.begin() + 3);
      }
      err << load_blob(filename, full_path, src_blob.data(), src_blob.size(), use_utf8_bom, verbose);

      break;
    }
  }

  if (!found) {
    err << ERR("Couldn't find %s in path!", filename);
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModLibrary::load_blob(const std::string &filename,
                                         const std::string &full_path,
                                         void* src_blob,
                                         int src_len,
                                         bool use_utf8_bom, bool verbose) {
  Err err;

  auto source_file = new MtSourceFile();
  err << source_file->init(this, filename, full_path, src_blob, src_len);
  source_file->use_utf8_bom = use_utf8_bom;
  add_source(source_file);

  // Recurse through #includes

  std::vector<std::string> includes;

  source_file->root_node.visit_tree([&](MnNode child) {
    if (child.sym != sym_preproc_include) return;

    std::string filename = child.get_field(field_path).text();
    filename.erase(filename.begin());
    filename.pop_back();
    includes.push_back(filename);
  });

  for (const auto &file : includes) {
    if (file == "metron_tools.h") continue;

    if (!get_source(file)) {
      MtSourceFile *source = nullptr;
      err << load_source(file.c_str(), source, verbose);
    }

    source_file->includes.push_back(get_source(file));
  }

  return err;
}

//------------------------------------------------------------------------------

void MtModLibrary::dump() {
  LOG_G("Mod library:\n");
  LOG_INDENT_SCOPE();
  for (auto s : source_files) {
    s->dump();
  }
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModLibrary::propagate(propagate_visitor v) {
  Err err;

  int passes = 0;
  int changes;
  do {
    passes++;
    changes = 0;
    for (auto mod : modules) {
      for (auto m : mod->all_methods) {
        changes += v(m);
      }
    }
  } while (changes);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModLibrary::categorize_methods() {
  Err err;

  //----------------------------------------
  // Trace done, all our fields should have a state assigned. Categorize the
  // methods.

  for (auto mod : modules) {
    for (auto m : mod->all_methods) {
      if (m->is_constructor()) {
        if (mod->constructor) {
          err << ERR("Module %s has multiple constructors\n", mod->cname());
        } else if (m->has_params()) {
          err << ERR("Constructor for %s is not allowed to have params\n",
                     mod->cname());
        } else {
          mod->constructor = m;
        }
      }
    }
  }

  //----------------------------------------
  // Mark all methods called by the constructor as inits

  err << propagate([&](MtMethod *m) {
    if (m->in_init) return 0;

    if (m->is_constructor()) {
      LOG_B("%-20s is init because it's the constructor.\n", m->cname());
      m->in_init = true;
      return 1;
    }

    for (auto caller : m->internal_callers) {
      if (caller->in_init) {
        if (m->in_init) {
          return 0;
        } else {
          LOG_B("%-20s is init because it's called by the constructor.\n",
                m->cname());
          m->in_init = true;
          return 1;
        }
      }
    }

    return 0;
  });

  //----------------------------------------
  // Methods that only call funcs in the same module and don't write anything
  // are funcs.

  err << propagate([&](MtMethod *m) {
    if (m->in_init) return 0;

    if (m->writes.empty() && m->external_callees.empty()) {
      bool only_calls_funcs = true;
      for (auto callee : m->internal_callees) {
        only_calls_funcs &= callee->in_func;
      }

      if (only_calls_funcs) {
        if (m->in_func) {
          return 0;
        } else {
          LOG_B(
              "%s.%s is func because it doesn't write anything and only calls "
              "other funcs.\n",
              m->_mod->cname(), m->cname());
          m->in_func = true;
          return 1;
        }
      }
    }

    return 0;
  });

  //----------------------------------------
  // Methods that call funcs in other modules _must_ be tocks.

  err << propagate([&](MtMethod *m) {
    if (m->in_init) return 0;
    if (m->in_func) return 0;

    if (m->external_callees.size()) {
      if (m->in_tock) {
        return 0;
      } else {
        LOG_B(
            "%s.%s is tock because it doesn't write anything and calls funcs "
            "in other modules.\n",
            m->_mod->cname(), m->cname());
        m->in_tock = true;
        return 1;
      }
    }

    return 0;
  });

  //----------------------------------------
  // Methods that write registers _must_ be ticks.

  err << propagate([&](MtMethod *m) {
    if (m->in_init) return 0;
    if (m->in_func) return 0;

    bool wrote_register = false;
    for (auto f : m->writes) {
      if (f->state == CTX_REGISTER || f->state == CTX_MAYBE)
        wrote_register = true;
    }

    if (wrote_register) {
      if (m->in_tick) {
        return 0;
      } else {
        LOG_B("%-20s is tick because it writes registers.\n", m->cname());
        m->in_tick = true;
        return 1;
      }
    }

    return 0;
  });

  //----------------------------------------
  // Methods that are downstream from ticks _must_ be ticks.

  err << propagate([&](MtMethod *m) {
    if (m->in_init) return 0;
    if (m->in_func) return 0;

    for (auto caller : m->internal_callers) {
      if (caller->in_tick) {
        if (m->in_tick) {
          return 0;
        } else {
          LOG_B("%-20s is tick because it is called by a tick.\n", m->cname());
          m->in_tick = true;
          return 1;
        }
      }
    }

    return 0;
  });

  //----------------------------------------
  // Methods that write signals _must_ be tocks.

  err << propagate([&](MtMethod *m) {
    if (m->in_init) return 0;
    if (m->in_func) return 0;

    bool wrote_signal = false;
    for (auto f : m->writes) {
      wrote_signal |= f->state == CTX_SIGNAL;
    }

    if (wrote_signal) {
      if (m->in_tock) {
        return 0;
      } else {
        LOG_B("%-20s is tock because it writes signals.\n", m->cname());
        m->in_tock = true;
        return 1;
      }
    }

    return 0;
  });

  //----------------------------------------
  // Methods that write outputs are tocks unless they're already ticks.

  err << propagate([&](MtMethod *m) {
    if (m->in_init) return 0;
    if (m->in_func) return 0;
    if (m->in_tick) return 0;

    bool wrote_output = false;
    for (auto f : m->writes) {
      wrote_output |= f->state == CTX_OUTPUT;
    }

    if (wrote_output) {
      if (m->in_tock) {
        return 0;
      } else {
        LOG_B(
            "%-20s is tock because it writes outputs and isn't already a "
            "tick.\n",
            m->cname());
        m->in_tock = true;
        return 1;
      }
    }

    return 0;
  });

  //----------------------------------------
  // Methods that are upstream from tocks _must_ be tocks.

  err << propagate([&](MtMethod *m) {
    if (m->in_init) return 0;
    if (m->in_func) return 0;

    for (auto &callee : m->internal_callees) {
      if (callee->in_tock) {
        if (m->in_tock) {
          return 0;
        } else {
          LOG_B("%-20s is tock because it calls a tock.\n", m->cname());
          m->in_tock = true;
          return 1;
        }
      }
    }

    return 0;
  });

  //----------------------------------------
  // Methods that are downstream from tocks are tocks unless they're already
  // ticks.

  err << propagate([&](MtMethod *m) {
    if (m->in_init) return 0;
    if (m->in_func) return 0;
    if (m->in_tick) return 0;

    for (auto caller : m->internal_callers) {
      if (caller->in_tock) {
        if (m->in_tock) {
          return 0;
        } else {
          LOG_B(
              "%-20s is tock because it its called by a tock and isn't already "
              "a tick.\n",
              m->cname());
          m->in_tock = true;
          return 1;
        }
      }
    }

    return 0;
  });

#if 0
  //----------------------------------------
  // If there are unmarked methods left, they must be upstream from a tick.
  // If they don't have return values, we can mark them as a tick so we can
  // reduce the total number of always_* blocks needed after conversion.

  err << propagate([&](MtMethod *m) {
    if (m->is_valid()) return 0;
    if (m->has_return()) return 0;

    for (auto &callee : m->callees) {
      if (callee->in_tick) {
        if (m->in_tick) {
          return 0;
        } else {
          LOG_B(
              "%-20s is tick because it hasn't been categorized yet, it has no "
              "return value, and it's upstream from a tick.\n",
              m->cname());
          m->in_tick = true;
          return 1;
        }
      }
    }

    return 0;
  });

  //----------------------------------------
  // If there are _still_ unmarked methods, they are tocks upstream from ticks
  // that have return values.

  err << propagate([&](MtMethod *m) {
    if (m->is_valid()) return 0;

    if (m->in_tock) {
      return 0;
    } else {
      m->in_tock = true;
      return 1;
    }
    return 0;
  });
#endif

  // Just mark everything left as tock.

  err << propagate([&](MtMethod *m) {
    if (m->is_valid()) return 0;

    if (m->in_tock) {
      return 0;
    } else {
      m->in_tock = true;
      return 1;
    }
    return 0;
  });

  //----------------------------------------
  // Methods categorized, we can split up internal_callers

  for (auto mod : modules) {
    for (auto m : mod->all_methods) {
      for (auto c : m->internal_callers) {
        if (c->in_tick) m->tick_callers.insert(c);
        if (c->in_tock) m->tock_callers.insert(c);
        if (c->in_func) m->func_callers.insert(c);
      }
    }
  }

  //----------------------------------------
  // Methods categorized, now we can categorize the inputs of the methods.

  for (auto mod : modules) {
    for (auto m : mod->all_methods) {
      if (!m->is_public()) continue;

      if (m->in_func || ((m->in_tick || m->in_tock) && m->internal_callers.empty())) {
        auto params =
            m->_node.get_field(field_declarator).get_field(field_parameters);
        for (const auto &param : params) {
          if (param.sym != sym_parameter_declaration) continue;
          MtFuncParam *new_input = new MtFuncParam(m->name(), param);
          mod->input_method_params.push_back(new_input);
        }
      }

      if (m->has_return()) {
        mod->output_method_returns.push_back(m);
      }
    }
  }



  //----------------------------------------
  // Check for ticks with return values.

  for (auto mod : modules) {
    for (auto m : mod->all_methods) {
      if (m->in_tick && m->has_return()) {
        err << ERR("Tick method %s.%s is not allowed to have a return value.\n",
                   mod->cname(), m->cname());
      }
    }
  }

  //----------------------------------------
  // Done!

  return err;
}

//------------------------------------------------------------------------------

// KCOV_OFF
void MtModLibrary::dump_call_graph() {
  LOG_G("Call graph:\n");

  std::function<void(MtModule *, MtMethod *)> dump_call_tree =
      [&](MtModule *mod, MtMethod *method) {
        uint32_t color = 0x808080;

        if (method->in_init) color = 0x8080FF;
        if (method->in_tick) color = 0x80FF80;
        if (method->in_tock) color = 0xFF8080;
        if (method->in_func) color = 0xFFFFFF;

        if (!method->is_valid()) color = 0x808080;

        LOG_C(color, " %s.%s()\n", mod->cname(), method->cname());

        LOG_INDENT_SCOPE();
        for (auto callee : method->internal_callees) {
          dump_call_tree(callee->_mod, callee);
        }
      };

  /*
  for (auto mod : modules) {
    if (mod->parents.size()) continue;
    for (auto method : mod->all_methods) {
      if (method->callers.empty()) {
        LOG_INDENT_SCOPE();
        dump_call_tree(mod, method);
      }
    }
  }
  */
}
// KCOV_ON

//------------------------------------------------------------------------------

#if 0
//----------------------------------------
// Dump stuff

if (parents.empty()) {
  LOG_B("Dumping %s trace\n", cname());
  LOG_INDENT_SCOPE();
  MtTracer::dump_trace(mod_state);
}

LOG_B("Dumping %s\n", cname());
LOG_INDENT_SCOPE();
for (auto method : all_methods) {
  if (method->in_init) continue;
  LOG_B("Dumping %s.%s\n", cname(), method->cname());
  LOG_INDENT_SCOPE();

  if (method->callers.empty()) {
    LOG_G("Root!\n");
  }

  if (method->callees.empty()) {
    LOG_G("Leaf!\n");
  }

  for (auto ref : method->callees) {
    LOG_G("Calls %s.%s\n", ref.mod->cname(), ref.method->cname());
  }
  for (auto ref : method->callers) {
    LOG_Y("Called by %s.%s\n", ref.mod->cname(), ref.method->cname());
  }

  for (const auto& ref : method->fields_read) {
    if (ref.subfield) {
      LOG_G("Reads %s.%s\n", ref.field->cname(), ref.subfield->cname());
    }
    else {
      LOG_G("Reads %s\n", ref.field->cname());
    }
  }

  for (const auto& ref : method->fields_written) {
    if (ref.subfield) {
      LOG_R("Writes %s.%s\n", ref.field->cname(), ref.subfield->cname());
    }
    else {
      LOG_R("Writes %s\n", ref.field->cname());
    }
  }
}

#endif
