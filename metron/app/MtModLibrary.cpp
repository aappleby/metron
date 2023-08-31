#include "metron/app/MtModLibrary.h"

#include "metron/app/MtSourceFile.h"
#include "metron/nodes/MtField.h"
#include "metron/nodes/MtFuncParam.h"
#include "metron/nodes/MtMethod.h"
#include "metron/nodes/MtModule.h"
#include "metron/nodes/MtStruct.h"
#include "metron/tracer/MtContext.h"

#include "metrolib/core/Log.h"

#include <filesystem>
#include <sys/stat.h>

namespace fs = std::filesystem;

//#pragma warning(disable : 4996) // unsafe fopen

//------------------------------------------------------------------------------

MtModule *MtModLibrary::get_module(const std::string &name) {
  for (auto mod : all_modules) {
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
  for (auto s : source_files) delete s;
  for (auto m : all_modules) delete m;
  for (auto s : all_structs) delete s;
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

std::string MtModLibrary::resolve_path(const std::string& filename) {
  for (auto &path : search_paths) {
    std::string full_path = path.size() ? path + "/" + filename : filename;
    if (fs::is_regular_file(full_path)) {
      return full_path;
    }
  }

  return "";
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModLibrary::load_source(const char *filename,
                                           MtSourceFile *&out_source) {
  Err err;

  if (get_source(filename)) {
    return WARN("Duplicate filename %s\n", filename);
  }

  std::string full_path = resolve_path(filename);

  if (full_path.empty()) {
    return ERR("Couldn't find %s in path!", filename);
  }

  LOG_B("Loading %s from %s\n", filename, full_path.c_str());
  LOG_INDENT_SCOPE();

  auto size = fs::file_size(full_path);

  std::string src_blob;
  src_blob.resize(size);

  auto f = fopen(full_path.c_str(), "rb");
  size_t result = fread((void *)src_blob.data(), 1, src_blob.size(), f);
  fclose(f);

  bool use_utf8_bom = false;
  if (uint8_t(src_blob[0]) == 239 &&
      uint8_t(src_blob[1]) == 187 &&
      uint8_t(src_blob[2]) == 191) {
    use_utf8_bom = true;
    src_blob.erase(src_blob.begin(), src_blob.begin() + 3);
  }

  auto source_file = new MtSourceFile();
  err << source_file->init(this, filename, full_path, src_blob, use_utf8_bom);
  add_source(source_file);

  // Recurse through #includes
  err << load_includes(source_file);

  out_source = source_file;


  return err;
}

//------------------------------------------------------------------------------
// Recurse through #includes

CHECK_RETURN Err MtModLibrary::load_includes(MtSourceFile* source_file) {
  Err err;
  std::vector<std::string> includes;

  source_file->root_node.visit_tree([&](MnNode child) {
    if (child.sym != sym_preproc_include) return;

    std::string filename = child.get_field(field_path).text();
    filename.erase(filename.begin());
    filename.pop_back();
    includes.push_back(filename);
  });

  for (const auto &file : includes) {
    if (file.find("metron_tools.h") != std::string::npos) continue;

    if (!get_source(file)) {
      MtSourceFile *source = nullptr;
      err << load_source(file.c_str(), source);
    }

    source_file->src_includes.push_back(get_source(file));
  }

  return err;
}

//------------------------------------------------------------------------------

void MtModLibrary::dump_lib() {
  LOG_G("Mod library:\n");
  LOG_INDENT_SCOPE();

  for (auto m : all_modules) m->dump_module();
  for (auto s : all_structs) s->dump_struct();
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModLibrary::propagate(propagate_visitor v) {
  Err err;

  int passes = 0;
  int changes;
  do {
    passes++;
    changes = 0;
    for (auto mod : all_modules) {
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
  for (auto s : all_structs) {
    if (s->name == name) return s;
  }
  return nullptr;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModLibrary::categorize_methods(bool verbose) {
  Err err;

  //----------------------------------------
  // Trace done, all our fields should have a state assigned. Categorize the
  // methods.

  for (auto mod : all_modules) {
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

  for (auto mod : all_modules) {
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

  for (auto mod : all_modules) {
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

  for (auto mod : all_modules) {
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

  for (auto mod : all_modules) {
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

  for (auto mod : all_modules) {
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
