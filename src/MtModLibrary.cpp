#include "MtModLibrary.h"

#include <sys/stat.h>

#include "Log.h"
#include "metron_tools.h"
#include "MtModule.h"
#include "MtSourceFile.h"

#pragma warning(disable : 4996)

//------------------------------------------------------------------------------

MtModule* MtModLibrary::get_module(const std::string& name) {
  assert(sources_loaded);
  for (auto mod : modules) {
    if (mod->mod_name == name) return mod;
  }
  return nullptr;
}

MtSourceFile* MtModLibrary::get_source(const std::string& name) {
  for (auto s : source_files) {
    if (s->filename == name) return s;
  }
  return nullptr;
}

//------------------------------------------------------------------------------

void MtModLibrary::teardown() {
  modules.clear();
  for (auto s : source_files) {
    delete s;
  }
}

//------------------------------------------------------------------------------

void MtModLibrary::add_search_path(const std::string& path) {
  assert(!sources_loaded);
  search_paths.push_back(path);
}

//------------------------------------------------------------------------------

void MtModLibrary::add_source(MtSourceFile* source_file) {
  assert(!sources_loaded);
  source_file->lib = this;
  source_files.push_back(source_file);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModLibrary::load_source(const char* filename, MtSourceFile*& out_source, bool verbose) {
  Err err;

  if (get_source(filename)) {
    return WARN("Duplicate filename %s\n", filename);
  }

  assert(!sources_loaded);
  bool found = false;
  for (auto& path : search_paths) {
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
      fread(src_blob.data(), 1, src_blob.size(), f);
      fclose(f);

      bool use_utf8_bom = false;
      if (uint8_t(src_blob[0]) == 239 && uint8_t(src_blob[1]) == 187 &&
          uint8_t(src_blob[2]) == 191) {
        use_utf8_bom = true;
        src_blob.erase(src_blob.begin(), src_blob.begin() + 3);
      }
      err << load_blob(filename, full_path, src_blob, use_utf8_bom, verbose);
      break;
    }
  }

  if (!found) {
    err << ERR("Couldn't find %s in path!", filename);
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModLibrary::load_blob(const std::string& filename, const std::string& full_path, const std::string& src_blob, bool use_utf8_bom, bool verbose) {
  Err err;

  assert(!sources_loaded);
  auto source_file = new MtSourceFile(this);
  err << source_file->init(filename, full_path, src_blob);
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

  for (const auto& file : includes) {
    if (file == "metron_tools.h") continue;

    if (!get_source(file)) {
      MtSourceFile* source = nullptr;
      err << load_source(file.c_str(), source, verbose);
    }

    source_file->includes.push_back(get_source(file));
  }

  return err;
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
  } while(changes);


  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModLibrary::process_sources() {
  Err err;

  assert(!sources_loaded);
  assert(!sources_processed);

  sources_loaded = true;
  
  for (auto s : source_files) {
    for (auto m : s->modules) {
      modules.push_back(m);
    }
  }

  //----------------------------------------
  // All modules are now in the library, we can resolve references to other
  // modules when we're collecting fields.

  for (auto mod : modules) {
    err << mod->collect_modparams();
    err << mod->collect_fields();
    err << mod->collect_methods();
  }

  //----------------------------------------
  // Hook up child->parent module pointers

  for (auto m : modules) {
    for (auto s : m->all_fields) {
      if (s->is_component()) {
        auto component = get_module(s->type_name());
        component->parents.push_back(m);
        m->children.push_back(component);
      }
    }
  }

  for (auto mod : modules) {
    err << mod->build_call_graph();
  }

  //----------------------------------------
  // Trace top modules

  //TinyLog::get().unmute();

  for (auto m : modules) {
    if (m->parents.empty()) {
      err << m->trace();
      LOG_Y("Trace:\n");
      for (const auto& pair : m->mod_state) {
        if (pair.second == FIELD_INVALID) {
          LOG_R("%s = %s\n", pair.first.c_str(), to_string(pair.second));
        }
        else {
          LOG("%s = %s\n", pair.first.c_str(), to_string(pair.second));
        }
      }
    }
  }

#if 0
  //----------------------------------------
  // Trace done, all our fields should have a state assigned. Categorize the methods.

  //----------------------------------------
  // Mark all methods called by the constructor as inits

  err << propagate([&](MtMethod* m) {
    if (m->in_init) return 0;
    
    for (auto caller : m->callers) {
      if (caller->in_init) {
        if (m->in_init) {
          return 0;
        }
        else {
          LOG_B("%-20s is init because it's called by the constructor.\n", m->cname());
          m->in_init = 1;
          return 1;
        }
      }
    }

    return 0;
  });


  //----------------------------------------
  // Methods that only call funcs and don't write anything are funcs.

  err << propagate([&](MtMethod* m) {
    if (m->in_init) return 0;

    if (!m->is_writer()) {

      bool only_calls_funcs = true;
      for (auto callee : m->callees) {
        only_calls_funcs &= callee->is_func;
      }

      if (only_calls_funcs) {

        if (m->is_func) {
          return 0;
        }
        else {
          LOG_B("%-20s is func because it doesn't write anything and only calls other funcs.\n", m->cname());
          m->is_func = true;
          return 1;
        }
      }
    }

    return 0;
  });

  //----------------------------------------
  // Methods that write registers _must_ be ticks.

  err << propagate([&](MtMethod* m) {
    if (m->in_init) return 0;
    if (m->is_func) return 0;

    bool wrote_register = false;
    for (auto f : m->fields_written) {
      if (f->state == FIELD_REGISTER) wrote_register = true;
    }

    if (wrote_register) {
      if (m->in_tick) {
        return 0;
      }
      else {
        LOG_B("%-20s is tick because it writes registers.\n", m->cname());
        m->in_tick = true;
        return 1;
      }
    }

    return 0;
    });

  //----------------------------------------
  // Methods that are downstream from ticks _must_ be ticks.

  err << propagate([&](MtMethod* m) {
    if (m->in_init) return 0;
    if (m->is_func) return 0;

    for (auto caller : m->callers) {
      if (caller->in_tick) {
        if (m->in_tick) {
          return 0;
        }
        else {
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

  err << propagate([&](MtMethod* m) {
    if (m->in_init) return 0;
    if (m->is_func) return 0;

    bool wrote_signal = false;
    for (auto f : m->fields_written) {
      wrote_signal  |= f->state == FIELD_SIGNAL;
    }

    if (wrote_signal) {
      if (m->in_tock) {
        return 0;
      }
      else {
        LOG_B("%-20s is tock because it writes signals.\n", m->cname());
        m->in_tock = true;
        return 1;
      }
    }

    return 0;
  });

  //----------------------------------------
  // Methods that write outputs are tocks unless they're already ticks.

  err << propagate([&](MtMethod* m) {
    if (m->in_init) return 0;
    if (m->is_func) return 0;
    if (m->in_tick) return 0;

    bool wrote_output = false;
    for (auto f : m->fields_written) {
      wrote_output  |= f->state == FIELD_OUTPUT;
    }

    if (wrote_output) {
      if (m->in_tock) {
        return 0;
      }
      else {
        LOG_B("%-20s is tock because it writes outputs and isn't already a tick.\n", m->cname());
        m->in_tock = true;
        return 1;
      }
    }

    return 0;
    });


  //----------------------------------------
  // Methods that are upstream from tocks _must_ be tocks.

  err << propagate([&](MtMethod* m) {
    if (m->in_init) return 0;
    if (m->is_func) return 0;

    for (auto& callee : m->callees) {
      if (callee->in_tock) {
        if (m->in_tock) {
          return 0;
        }
        else {
          LOG_B("%-20s is tock because it calls a tock.\n", m->cname());
          m->in_tock = true;
          return 1;
        }
      }
    }

    return 0;
    });

  //----------------------------------------
  // Methods that are downstream from tocks are tocks unless they're already ticks.

  err << propagate([&](MtMethod* m) {
    if (m->in_init) return 0;
    if (m->is_func) return 0;
    if (m->in_tick) return 0;

    for (auto caller : m->callers) {
      if (caller->in_tock) {
        if (m->in_tock) {
          return 0;
        }
        else {
          LOG_B("%-20s is tock because it its called by a tock and isn't already a tick.\n", m->cname());
          m->in_tock = true;
          return 1;
        }
      }
    }

    return 0;
    });


  //----------------------------------------
  // If there are unmarked methods left, they must be upstream from a tick.
  // Mark them as a tick so we can reduce the total number of always_* blocks
  // needed after conversion.
  // *** can't turn them into ticks if they have return vals

  err << propagate([&](MtMethod* m) {
    if (m->is_valid()) return 0;

    for (auto& callee : m->callees) {
      if (callee->in_tick) {
        if (m->in_tick) {
          return 0;
        }
        else {
          LOG_B("%-20s is tick because it hasn't been categorized yet and is upstream from a tick.\n", m->cname());
          m->in_tick = true;
          return 1;
        }
      }
    }

    return 0;
  });


  //----------------------------------------

  int uncategorized = 0;
  int invalid = 0;
  for (auto mod : modules) {
    for (auto m : mod->all_methods) {
      if (!m->categorized()) {
        m->categorize();
        uncategorized++;
      }
      if (!m->is_valid()) {
        invalid++;
      }
    }
  }

  LOG_G("Methods uncategorized %d\n", uncategorized);
  LOG_G("Methods invalid %d\n", invalid);

  //----------------------------------------
  // Everything's categorized now. Next step - check for duplicate method bindings.

  // This isnt' right, it needs to be "max 1 binding per branch"...

#if 0

  for (auto src_mod : modules) {
    for (auto src_method : src_mod->all_methods) {
      for (auto& ref : src_method->callees) {
        auto dst_mod = ref.mod;
        auto dst_method = ref.method;

        if (src_mod != dst_mod) {
          // Cross-module calls always require binding.
          dst_method->binding_count++;
        }
        else if (src_method->in_tock && dst_method->in_tick) {
          // Cross-domain calls always require binding.
          dst_method->binding_count++;
        }
      }
    }
  }

  for (auto src_mod : modules) {
    for (auto src_method : src_mod->all_methods) {
      if (src_method->binding_count > 1) {
        err << ERR("Duplicate bindings for %s.%s\n", src_mod->cname(), src_method->cname());
      }
    }
  }

#endif

  //----------------------------------------
  // Check for cross-module calls in ticks.

  for (auto src_mod : modules) {
    for (auto src_method : src_mod->all_methods) {
      for (auto callee : src_method->callees) {
        auto dst_mod = callee->mod;
        auto dst_method = callee;

        if (src_mod != dst_mod) {
          if (src_method->in_tick) {
            err << ERR("Calling from tick %s.%s to %s.%s crosses a module boundary, but ticks can't create bindings\n",
              src_mod->cname(), src_method->cname(), dst_mod->cname(), dst_method->cname());
          }
        }
      }
    }
  }

  //----------------------------------------
  // Check for ticks with return values.

  for (auto mod : modules) {
    for (auto m : mod->all_methods) {
      if (m->in_tick && m->has_return) {
        err << ERR("Tick method %s.%s is not allowed to have a return value.\n", mod->cname(), m->cname());
      }
    }
  }

  //----------------------------------------
  // Check for constructors with params

  for (auto mod : modules) {
    if (mod->constructor && mod->constructor->params.size()) {
      err << ERR("Constructor for %s is not allowed to have params\n", mod->cname());
    }
  }

  //----------------------------------------

  dump_call_graph();

  if (err.has_err()) {
    exit(1);
  }

  exit((uncategorized > 0) || (invalid > 0));

  //----------------------------------------
  // All modules have populated their fields, match up tick/tock calls with their
  // corresponding methods.

  for (auto mod : modules) {
    err << mod->sort_fields();
    //err << mod->build_port_map();
  }

  //----------------------------------------

  sources_processed = true;

#endif

  return err;
}

//------------------------------------------------------------------------------

void MtModLibrary::dump_call_graph() {
  LOG_G("Call graph:\n");

  std::function<void(MtModule*, MtMethod*)> dump_call_tree = [&](MtModule* mod, MtMethod* method) {
    uint32_t color = 0x808080;

    if (method->in_init) color = 0x8080FF;
    if (method->in_tick) color = 0x80FF80;
    if (method->in_tock) color = 0xFF8080;
    if (method->in_func) color = 0xFFFFFF;

    if (!method->is_valid()) color = 0x808080;

    LOG_C(color, " %s.%s()\n", mod->cname(), method->cname());


    LOG_INDENT_SCOPE();
    for (auto callee : method->callees) {
      dump_call_tree(callee->_mod, callee);
    }
  };

  for (auto mod : modules) {
    if (mod->parents.size()) continue;
    for (auto method : mod->all_methods) {
      if (method->callers.empty()) {
        LOG_INDENT_SCOPE();
        dump_call_tree(mod, method);
      }
    }
  }
}

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

