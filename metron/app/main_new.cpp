#include "MetronApp.h"

#include "matcheroni/Utilities.hpp"
#include "metrolib/core/Log.h"
#include "metrolib/core/Platform.h"
#include "metron/parser/CInstance.hpp"
#include "metron/parser/CNodeClass.hpp"
#include "metron/parser/CNodeField.hpp"
#include "metron/parser/CNodeFunction.hpp"
#include "metron/parser/CNodeStruct.hpp"
#include "metron/parser/CParser.hpp"
#include "metron/parser/CSourceFile.hpp"
#include "metron/parser/CSourceRepo.hpp"
#include "metron/parser/Cursor.hpp"
#include "metron/parser/NodeTypes.hpp"
#include "metron/parser/Tracer.hpp"
#include "metron/tools/MtUtils.h"

using namespace matcheroni;

//------------------------------------------------------------------------------

static std::vector<std::string> split_path(const std::string& input) {
  std::vector<std::string> result;
  std::string temp;

  const char* c = input.c_str();

  do {
    if (*c == '/' || *c == '\\' || *c == 0) {
      if (temp.size()) result.push_back(temp);
      temp.clear();
    } else {
      temp.push_back(*c);
    }
  } while (*c++ != 0);

  return result;
}

//------------------------------------------------------------------------------

static std::string join_path(std::vector<std::string>& path) {
  std::string result;
  for (auto& s : path) {
    if (result.size()) result += "/";
    result += s;
  }
  return result;
}

//------------------------------------------------------------------------------

static void mkdir_all(const std::vector<std::string>& full_path) {
  std::string temp;
  for (size_t i = 0; i < full_path.size(); i++) {
    if (temp.size()) temp += "/";
    temp += full_path[i];
    //printf("making dir %s\n", temp.c_str());
    int result = plat_mkdir(temp.c_str());
    //printf("mkdir result %d\n", result);
  }
}

//------------------------------------------------------------------------------

int main_new(Options opts) {
  LOG_G("New parser!\n");

  Err err;

  CSourceRepo repo;
  CSourceFile* root_file = nullptr;
  err << repo.load_source(opts.src_name, &root_file);

  if (!root_file) {
    LOG_R("Could not load %s\n", opts.src_name.c_str());
    return -1;
  }

  //----------------------------------------
  // All modules are now in the library, we can resolve references to other
  // modules when we're collecting fields.

  LOG_B("//----------------------------------------\n");
  LOG_B("Processing source files\n");

  LOG_B("collect_fields_and_methods\n");

  for (auto pair : repo.source_map) {
    CSourceFile* file = pair.second;

    for (auto n : file->context.root_node) {

      if (auto node_template = n->as_a<CNodeTemplate>()) {
        auto node_class = node_template->child<CNodeClass>();

        node_class->repo = &repo;
        node_class->file = file;

        repo.all_classes.push_back(node_class);
        node_class->collect_fields_and_methods();
      }
      else if (auto node_class = n->as_a<CNodeClass>()) {

        node_class->repo = &repo;
        node_class->file = file;

        repo.all_classes.push_back(node_class);
        node_class->collect_fields_and_methods();
      }
      else if (auto node_struct = n->as_a<CNodeStruct>()) {
        node_struct->repo = &repo;
        node_struct->file = file;

        repo.all_structs.push_back(node_struct);
        node_struct->collect_fields_and_methods();
      }
      else if (auto node_namespace = n->as_a<CNodeNamespace>()) {
        node_namespace->repo = &repo;
        node_namespace->file = file;

        repo.all_namespaces.push_back(node_namespace);
        node_namespace->collect_fields_and_methods();
      }

    }
  }

  //----------------------------------------
  // Count module instances so we can find top modules.

  for (auto c : repo.all_classes) {
    for (auto f : c->all_fields) {
      if (f->is_component()) {
        f->_type_class->refcount++;
      }
    }
  }

  //----------------------------------------

  LOG_B("build_call_graphs\n");
  for (auto mod : repo.all_classes) {
    err << mod->build_call_graph(&repo);
  }

  //----------------------------------------
  // Trace

  {
    LOG_B("//----------------------------------------\n");
    LOG_B("Tracing top methods\n");
    LOG_INDENT_SCOPE();

    for (auto node_class : repo.all_classes) {
      auto name = node_class->get_name();
      if (node_class->refcount) {
        LOG_G("Skipping %.*s because it's not a top module\n", int(name.size()), name.data());
        continue;
      }

      LOG_G("Tracing top methods in %.*s\n", int(name.size()), name.data());
      auto top_inst = new CInstClass(nullptr, nullptr, node_class);

      for (auto node_func : node_class->all_functions) {

        LOG_INDENT_SCOPE();
        auto func_name = node_func->get_name();
        if (node_func->internal_callers.size()) {
          LOG_B("Skipping %.*s because it's not toplevel\n", int(func_name.size()), func_name.data());
        }
        else if (node_func->method_type == MT_INIT) {
          LOG_B("Skipping %.*s because it's an initializer\n", int(func_name.size()), func_name.data());
        }
        else {
          LOG_B("Tracing %.*s\n", int(func_name.size()), func_name.data());

          auto top_call = new CCall(top_inst, nullptr, node_func);
          top_inst->entry_points.push_back(top_call);
          err << node_func->trace(top_call);
        }
      }

      LOG_G("Tracing done for %.*s\n", int(name.size()), name.data());
      top_inst->commit_state();
      delete top_inst;
    }
  }

  for (auto c : repo.all_classes) {
    for (auto f : c->all_functions) {
      f->propagate_rw();
    }
  }

  //----------------------------------------

  LOG_B("Categorizing fields\n");

  for (auto c : repo.all_classes) {
    for (auto f : c->all_fields) {
      if (f->_type_class) {
        f->field_type = FT_MODULE;
      }
    }
  }

  for (auto c : repo.all_classes) {
    auto cname = c->get_name();
    for (auto f : c->all_fields) {
      auto fname = f->get_name();
      if (f->field_type == FT_UNKNOWN) {
        LOG_R("Field %.*s::%.*s has no type\n", cname.size(), cname.data(), fname.size(), fname.data());
        assert(false);
      }
    }
  }

  for (auto c : repo.all_classes) {
    if (c->all_constructors.size() > 1) {
      auto name = c->get_name();
      err << ERR("Class %.*s has multiple constructors", int(name.size()), name.data());
    }
  }

  //========================================

  LOG_B("Categorizing methods\n");

  //----------------------------------------
  // Everything called by a constructor is init.

  for (auto c : repo.all_classes) {
    for (auto f : c->all_constructors) {
      f->set_type(MT_INIT);
      f->visit_internal_callees([](CNodeFunction* f){
        f->set_type(MT_INIT);
      });
    }
  }

  //----------------------------------------
  // Methods named "tick" are ticks, etc.

  for (auto c : repo.all_classes) {
    for (auto f : c->all_functions) {
      if (f->get_name().starts_with("tick")) f->set_type(MT_TICK);
      if (f->get_name().starts_with("tock")) f->set_type(MT_TOCK);
    }
  }

  //----------------------------------------
  // Methods that only call funcs in the same module and don't write anything
  // are funcs.

  for (auto c : repo.all_classes) {
    for (auto f : c->all_functions) {
      if (f->all_writes.empty() && f->external_callees.empty()) f->set_type(MT_FUNC);
    }
  }

  //----------------------------------------
  // Methods that call funcs in other modules _must_ be tocks.

  for (auto c : repo.all_classes) {
    for (auto f : c->all_functions) {
      if (f->external_callees.size()) f->set_type(MT_TOCK);
    }
  }

  //----------------------------------------
  // Methods that write registers _must_ be ticks.

  for (auto c : repo.all_classes) {
    for (auto f : c->all_functions) {
      bool writes_reg = false;
      for (auto w : f->all_writes) {
        if (w->field_type == FT_REGISTER) writes_reg = true;
      }

      if (writes_reg) f->set_type(MT_TICK);
    }
  }

  //----------------------------------------
  // Methods that are downstream from ticks _must_ be ticks.

  for (auto c : repo.all_classes) {
    for (auto f : c->all_functions) {
      if (f->method_type == MT_TICK) {
        f->visit_internal_callees([](CNodeFunction* f2) {
          f2->set_type(MT_TICK);
        });
        assert(f->external_callees.empty());
      }
    }
  }

  //----------------------------------------
  // Methods that write signals _must_ be tocks.

  for (auto c : repo.all_classes) {
    for (auto f : c->all_functions) {
      bool writes_sig = false;
      for (auto w : f->all_writes) {
        if (w->field_type == FT_SIGNAL) writes_sig = true;
      }

      if (writes_sig) f->set_type(MT_TOCK);
    }
  }

  //----------------------------------------
  // Methods that are upstream from tocks _must_ be tocks.

  for (auto c : repo.all_classes) {
    for (auto f : c->all_functions) {
      if (f->method_type == MT_TOCK) {
        f->visit_internal_callers([](CNodeFunction* f2) {
          f2->set_type(MT_TOCK);
        });
        f->visit_external_callers([](CNodeFunction* f2) {
          f2->set_type(MT_TOCK);
        });
      }
    }
  }

  //----------------------------------------
  // Methods that write outputs are tocks unless they're already ticks.

  //----------------------------------------
  // Methods that are downstream from tocks are tocks unless they're already
  // ticks.

  //----------------------------------------
  // Just mark everything left as tock.

  for (auto c : repo.all_classes) {
    for (auto f : c->all_functions) {
      assert(f->method_type != MT_UNKNOWN);
    }
  }

  //----------------------------------------
  // Methods categorized, we can assign emit types

  //----------------------------------------
  // Methods categorized, we can split up internal_callers

  //----------------------------------------
  // Methods categorized, now we can categorize the inputs of the methods.

  //----------------------------------------
  // Check for ticks with return values.

  //----------------------------------------
  // Done!
  //----------------------------------------

  {
  }

  //----------------------------------------

  if (opts.verbose) {
    LOG_B("\n");
    LOG_B("//----------------------------------------\n");
    LOG_B("// Repo dump\n\n");
    LOG_INDENT_SCOPE();
    repo.dump();
    LOG_B("//----------------------------------------\n");
    LOG_B("\n");
  }


  exit(0);


  //----------------------------------------
  // Emit all modules.

  LOG_G("Converting %s to SystemVerilog\n", opts.src_name.c_str());

  std::string out_string;
  Cursor cursor(&repo, root_file, &out_string);
  cursor.echo = opts.echo && !opts.quiet;

  if (opts.echo) LOG_G("\n----------------------------------------\n");
  err << cursor.emit_everything();
  err << cursor.emit_trailing_whitespace();
  if (opts.echo) LOG_G("----------------------------------------\n\n");

  if (err.has_err()) {
    LOG_R("Error during code generation\n");
    //lib.teardown();
    return -1;
  }

  // Save translated source to output directory, if there is one.
  if (opts.dst_name.size()) {
    LOG_G("Saving %s\n", opts.dst_name.c_str());

    auto dst_path = split_path(opts.dst_name);
    dst_path.pop_back();
    mkdir_all(dst_path);

    FILE* out_file = fopen(opts.dst_name.c_str(), "wb");
    if (!out_file) {
      LOG_R("ERROR Could not open %s for output\n", opts.dst_name.c_str());
    } else {
      // Copy the BOM over if needed.
      if (root_file->use_utf8_bom) {
        uint8_t bom[3] = {239, 187, 191};
        fwrite(bom, 1, 3, out_file);
      }

      fwrite(out_string.data(), 1, out_string.size(), out_file);
      fclose(out_file);
    }
  }

  //----------------------------------------

  LOG("Done!\n");

  return 0;
}
