#include "MtModLibrary.h"

#include <sys/stat.h>

#include "Log.h"
#include "MtField.h"
#include "MtFuncParam.h"
#include "MtMethod.h"
#include "MtModule.h"
#include "MtContext.h"
#include "MtSourceFile.h"
#include "MtTracer.h"
#include "MtStruct.h"
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

std::vector<std::string> split_path(const std::string& input);

CHECK_RETURN Err MtModLibrary::load_source(const char *filename,
                                           MtSourceFile *&out_source) {
  Err err;

  if (!std::string(filename).ends_with(".h")) {
    return err << ERR("Source file %s does not end with .h\n", filename);
  }

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
      LOG_B("Loading %s from %s\n", filename, full_path.c_str());
      LOG_INDENT_SCOPE();

      std::string src_blob;
      src_blob.resize(s.st_size);

      auto f = fopen(full_path.c_str(), "rb");
      size_t result = fread((void *)src_blob.data(), 1, src_blob.size(), f);
      fclose(f);

      bool use_utf8_bom = false;
      if (uint8_t(src_blob[0]) == 239 && uint8_t(src_blob[1]) == 187 &&
          uint8_t(src_blob[2]) == 191) {
        use_utf8_bom = true;
        src_blob.erase(src_blob.begin(), src_blob.begin() + 3);
      }
      err << load_blob(filename, full_path, src_blob.data(), src_blob.size(), out_source, use_utf8_bom);

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
                                         MtSourceFile*& out_source,
                                         bool use_utf8_bom) {
  Err err;

  auto source_file = new MtSourceFile();
  err << source_file->init(this, filename, full_path, src_blob, src_len);
  source_file->use_utf8_bom = use_utf8_bom;
  add_source(source_file);

  // Recurse through #includes

  std::vector<std::string> includes;

  bool noconvert = false;
  bool dumpit = false;

  source_file->root_node.visit_tree([&](MnNode child) {
    if (child.sym == sym_comment && child.contains("metron_noconvert")) {
      noconvert = true;
      return;
    }

    if (noconvert) {
      noconvert = false;
      return;
    }

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
      err << load_source(file.c_str(), source);
    }

    source_file->includes.push_back(get_source(file));
  }

  out_source = source_file;

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
        if (!m->categorized()) {
          changes += v(m);
        }
      }
    }
  } while (changes);

  return err;
}

//------------------------------------------------------------------------------

MtStruct* MtModLibrary::get_struct(const std::string& name) const {
  for (auto s : structs) {
    if (s->name == name) return s;
  }
  return nullptr;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModLibrary::collect_structs() {
  Err err;

  for (auto source : source_files) {
    for (auto s : source->src_structs) {
      structs.push_back(s);
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModLibrary::categorize_methods(bool verbose) {
  Err err;

  //----------------------------------------
  // Trace done, all our fields should have a state assigned. Categorize the
  // methods.

  for (auto mod : modules) {
    for (auto m : mod->all_methods) {
      if (m->is_constructor()) {
        if (mod->constructor) {
          err << ERR("Module %s has multiple constructors\n", mod->cname());
        } else {
          mod->constructor = m;
        }
      }
    }
  }

  //----------------------------------------
  // Methods named "tick" are ticks, etc.

  for (auto mod : modules) {
    for (auto m : mod->all_methods) {
      //if (m->name().starts_with("init")) m->is_init_ = true;
      if (m->name().starts_with("tick")) m->is_tick_ = true;
      if (m->name().starts_with("tock")) m->is_tock_ = true;
    }
  }

  //----------------------------------------
  // Mark all methods called by the constructor as inits

  err << propagate([&](MtMethod *m) {
    if (m->is_constructor()) {
      if (verbose) LOG_B("%s.%s is init because it's the constructor.\n", m->_mod->cname(), m->cname());
      m->is_init_ = true;
      return 1;
    }

    for (auto caller : m->internal_callers) {
      if (caller->is_init_) {
        if (verbose) LOG_B("%s.%s is init because it's called by the constructor.\n",
              m->_mod->cname(), m->cname());
        m->is_init_ = true;
        return 1;
      }
    }

    return 0;
  });

  //----------------------------------------
  // Methods that only call funcs in the same module and don't write anything
  // are funcs.

  err << propagate([&](MtMethod *m) {
    if (m->writes.empty() && m->external_callees.empty()) {
      bool only_calls_funcs = true;
      for (auto callee : m->internal_callees) {
        only_calls_funcs &= callee->is_func_;
      }

      if (only_calls_funcs) {
        if (verbose) LOG_B(
            "%s.%s is func because it doesn't write anything and only calls "
            "other funcs.\n",
            m->_mod->cname(), m->cname());
        m->is_func_ = true;
        return 1;
      }
    }

    return 0;
  });

  //----------------------------------------
  // Methods that call funcs in other modules _must_ be tocks.

  err << propagate([&](MtMethod *m) {
    if (m->external_callees.size()) {
      if (verbose) LOG_B("%s.%s is tock because it calls methods in other modules.\n", m->_mod->cname(), m->cname());
      m->is_tock_ = true;
      return 1;
    }
    return 0;
  });

  //----------------------------------------
  // Methods that write registers _must_ be ticks.

  err << propagate([&](MtMethod *m) {
    for (auto f : m->writes) {
      if (f->state() == CTX_REGISTER || f->state() == CTX_MAYBE) {
        if (verbose) LOG_B("%s.%s is tick because it writes registers.\n", m->_mod->cname(), m->cname());
        m->is_tick_ = true;
        return 1;
      }
    }
    return 0;
  });

  //----------------------------------------
  // Methods that are downstream from ticks _must_ be ticks.

  err << propagate([&](MtMethod *m) {
    for (auto caller : m->internal_callers) {
      if (caller->is_tick_) {
        if (verbose) LOG_B("%s.%s is tick because it is called by a tick.\n", m->_mod->cname(), m->cname());
        m->is_tick_ = true;
        return 1;
      }
    }

    return 0;
  });

  //----------------------------------------
  // Methods that write signals _must_ be tocks.

  err << propagate([&](MtMethod *m) {
    for (auto f : m->writes) {
      if (f->state() == CTX_SIGNAL) {
        if (verbose) LOG_B("%s.%s is tock because it writes signals.\n", m->_mod->cname(), m->cname());
        m->is_tock_ = true;
        return 1;
      }
    }
    return 0;
  });

  //----------------------------------------
  // Methods that are upstream from tocks _must_ be tocks.

  err << propagate([&](MtMethod *m) {
    for (auto &callee : m->internal_callees) {
      if (callee->is_tock_) {
        if (verbose) LOG_B("%s.%s is tock because it calls a tock.\n", m->_mod->cname(), m->cname());
        m->is_tock_ = true;
        return 1;
      }
    }
    return 0;
  });

  //----------------------------------------
  // Methods that write outputs are tocks unless they're already ticks.

  err << propagate([&](MtMethod *m) {
    for (auto f : m->writes) {
      if (f->state() == CTX_OUTPUT) {
        if (verbose) LOG_B(
            "%-20s is tock because it writes outputs and isn't already a "
            "tick.\n",
            m->cname());
        m->is_tock_ = true;
        return 1;
      }
    }
    return 0;
  });

  //----------------------------------------
  // Methods that are downstream from tocks are tocks unless they're already
  // ticks.

  err << propagate([&](MtMethod *m) {
    for (auto caller : m->internal_callers) {
      if (caller->is_tock_) {
        if (verbose) LOG_B(
            "%-20s is tock because it its called by a tock and isn't already "
            "a tick.\n",
            m->cname());
        m->is_tock_ = true;
        return 1;
      }
    }
    return 0;
  });

  //----------------------------------------
  // Just mark everything left as tock.

  err << propagate([&](MtMethod *m) {
    LOG_B("Forcing %s to tock\n", m->cname());
    m->is_tock_ = true;
    return 1;
  });

  //----------------------------------------
  // Methods categorized, we can assign emit types

  for (auto mod : modules) {
    for (auto m : mod->all_methods) {

      if (m->is_constructor()) {
        m->emit_as_init = true;
      }
      else if (m->called_in_init()) {
        m->emit_as_task = true;
      }
      else if (m->is_tick_) {
        m->emit_as_always_ff = !m->called_in_tick();
        m->emit_as_task      =  m->called_in_tick();
        m->needs_ports       = !m->called_in_module();
        m->needs_binding     =  m->called_by_tock();
      }
      else if (m->is_tock_) {
        m->emit_as_always_comb = true;
        m->needs_binding       =  m->called_in_module();
        m->needs_ports         = !m->called_in_module();
      }
      else if (m->is_func_) {
        if (m->is_public() && !m->called_in_module()) {
          m->needs_ports = true;
          m->emit_as_always_comb = true;
        }
        else {
          m->emit_as_func = true;
        }
      }
      else {
        err << ERR("wat\n");
      }
    }
  }

  //----------------------------------------
  // Methods categorized, we can split up internal_callers

  for (auto mod : modules) {
    for (auto m : mod->all_methods) {
      for (auto c : m->internal_callers) {
        if (c->is_tick_) m->tick_callers.insert(c);
        if (c->is_tock_) m->tock_callers.insert(c);
        if (c->is_func_) m->func_callers.insert(c);
      }
    }
  }

  //----------------------------------------
  // Methods categorized, now we can categorize the inputs of the methods.

  for (auto mod : modules) {
    for (auto m : mod->all_methods) {
      if (!m->is_public()) continue;

      if (m->is_func_ || ((m->is_tick_ || m->is_tock_) && m->internal_callers.empty())) {
        auto params = m->_node.get_field(field_declarator).get_field(field_parameters);
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
      if (m->is_tick_ && m->has_return()) {
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

        if (method->is_init_) color = 0x8080FF;
        if (method->is_tick_) color = 0x80FF80;
        if (method->is_tock_) color = 0xFF8080;
        if (method->is_func_) color = 0xFFFFFF;

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
