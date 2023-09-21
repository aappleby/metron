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
  // Sanity check parse tree

  for (auto pair : repo.source_map) {
    CSourceFile* file = pair.second;

    // Sanity check that all node spans in the repo tightly contain their
    // children
    node_visitor check_span = [](CNode* n) {
      if (n->child_head) {
        auto text = n->get_text();
        if (n->span.begin != n->child_head->span.begin) {
          LOG("%.*s\n", text.size(), text.data());
          assert(false);
        }
        if (n->span.end != n->child_tail->span.end) {
          LOG("%.*s\n", text.size(), text.data());
          assert(false);
        }
      }
    };

    visit_children(file->context.root_node, check_span);
  }
  LOG("\n");

  //----------------------------------------

  /*
  for (auto pair : repo.source_map) {
    CSourceFile* file = pair.second;
    file->context.top_head->dump_parse_tree();
  }
  LOG("\n");
  */

  //----------------------------------------
  // All modules are now in the library, we can resolve references to other
  // modules when we're collecting fields.

  LOG_B("//----------------------------------------\n");
  LOG_B("// Processing source files\n");

  for (auto pair : repo.source_map) {
    CSourceFile* file = pair.second;

    for (auto n : file->context.root_node) {
      if (auto node_template = n->as<CNodeTemplate>()) {
        auto node_class = node_template->child<CNodeClass>();
        node_class->repo = &repo;
        node_class->file = file;
        repo.all_classes.push_back(node_class);
      }
      else if (auto node_class = n->as<CNodeClass>()) {
        node_class->repo = &repo;
        node_class->file = file;
        repo.all_classes.push_back(node_class);
      }
      else if (auto node_struct = n->as<CNodeStruct>()) {
        node_struct->repo = &repo;
        node_struct->file = file;
        repo.all_structs.push_back(node_struct);
      }
      else if (auto node_namespace = n->as<CNodeNamespace>()) {
        node_namespace->repo = &repo;
        node_namespace->file = file;
        repo.all_namespaces.push_back(node_namespace);
      }
      else if (auto node_enum = n->as<CNodeEnum>()) {
        node_enum->repo = &repo;
        node_enum->file = file;
        repo.all_enums.push_back(node_enum);
      }

    }
  }

  LOG_B("collect_fields_and_methods\n");

  for (auto pair : repo.source_map) {
    CSourceFile* file = pair.second;

    for (auto n : file->context.root_node) {

      if (auto node_template = n->as<CNodeTemplate>()) {
        auto node_class = node_template->child<CNodeClass>();
        node_class->collect_fields_and_methods();
      }
      else if (auto node_class = n->as<CNodeClass>()) {
        node_class->collect_fields_and_methods();
      }
      else if (auto node_struct = n->as<CNodeStruct>()) {
        node_struct->collect_fields_and_methods();
      }
      else if (auto node_namespace = n->as<CNodeNamespace>()) {
        node_namespace->collect_fields_and_methods();
      }
      else if (auto node_enum = n->as<CNodeEnum>()) {
        //LOG_G("top level enum!!!!\n");
      }

    }
  }

  //----------------------------------------
  // Count module instances so we can find top modules.

  for (auto c : repo.all_classes) {
    for (auto f : c->all_fields) {
      if (f->node_decl->_type_class) f->node_decl->_type_class->refcount++;
    }
  }

  //----------------------------------------

  LOG_B("Building call graph\n");
  for (auto node_class : repo.all_classes) {
    err << node_class->build_call_graph(&repo);
  }

  for (auto node_class : repo.all_classes) {
    for (auto node_func : node_class->all_functions) {
      if (node_func->is_public && node_func->internal_callers.size()) {
        LOG_R("Function %s is public and has internal callers\n", node_func->name.c_str());
        assert(false);
      }
      //err << node_class->build_call_graph(&repo);
    }
  }

  //----------------------------------------

  LOG_B("Instantiating modules\n");
  for (auto node_class : repo.all_classes) {
    auto instance = new CInstClass(node_class->get_namestr(), true, nullptr, nullptr, node_class);
    repo.all_instances.push_back(instance);
  }
  LOG_B("\n");

  //----------------------------------------
  // Trace

  {
    LOG_B("//----------------------------------------\n");
    LOG_B("// Tracing classes\n");
    LOG_B("\n");
    LOG_INDENT_SCOPE();

    for (auto inst_class : repo.all_instances) {
      auto node_class = inst_class->node_class;
      auto name = node_class->get_name();
      LOG_G("Tracing public methods in %.*s\n", int(name.size()), name.data());

      if (auto node_func = node_class->constructor) {
        LOG_INDENT_SCOPE();
        auto func_name = node_func->get_namestr();

        if (!node_func->is_public) {
          LOG_B("Skipping %s because it's not public\n", func_name.c_str());
        }
        else {
          LOG_B("Tracing %s\n", func_name.c_str());
          auto inst_func = inst_class->resolve(func_name);
          call_stack stack;
          stack.push_back(node_func);
          err << node_func->trace(inst_func, stack);
        }
      }

      for (auto node_func : node_class->all_functions) {
        LOG_INDENT_SCOPE();
        auto func_name = node_func->get_namestr();
        if (!func_name.starts_with("tock")) continue;
        if (!node_func->is_public) continue;

        LOG_B("Tracing %s\n", func_name.c_str());
        auto inst_func = inst_class->resolve(func_name);
        call_stack stack;
        stack.push_back(node_func);
        err << node_func->trace(inst_func, stack);
      }

      for (auto node_func : node_class->all_functions) {
        LOG_INDENT_SCOPE();
        auto func_name = node_func->get_namestr();
        if (!func_name.starts_with("tick")) continue;
        if (!node_func->is_public) continue;

        LOG_B("Tracing %s\n", func_name.c_str());
        auto inst_func = inst_class->resolve(func_name);
        call_stack stack;
        stack.push_back(node_func);
        err << node_func->trace(inst_func, stack);
      }

      inst_class->commit_state();

      LOG_G("Tracing done for %.*s\n", int(name.size()), name.data());
    }

    for (auto c : repo.all_classes) {
      if (c->constructor) {
        c->constructor->propagate_rw();
      }
      for (auto f : c->all_functions) {
        f->propagate_rw();
      }
    }

    LOG_B("All tracing done\n");
    LOG_B("\n");
  }

  //----------------------------------------

  for (auto node_class : repo.all_classes) {
    for (auto f : node_class->all_functions) {
      auto method_type = f->get_method_type();

      if (method_type != MT_FUNC &&
          method_type != MT_TICK &&
          method_type != MT_TOCK &&
          method_type != MT_INIT) {
        assert(false);
      }

      f->method_type = method_type;
    }
    LOG_G("get_method_type OK\n");
  }
  LOG("\n");

  //----------------------------------------

  {
    LOG_B("//----------------------------------------\n");
    LOG_B("// Checking port compatibility\n");
    LOG_B("\n");

    for (auto inst : repo.all_instances) {
      LOG("Module %s\n", inst->name.c_str());
      LOG_INDENT_SCOPE();
      for (auto child : inst->children) {
        if (auto inst_submod = child->as<CInstClass>()) {
          //LOG("Submod %s\n", inst_submod->name.c_str());
          //LOG("Submod class %s\n", inst_submod->node_class->get_namestr().c_str());
          auto inst_b = repo.get_instance(inst_submod->node_class->get_namestr());
          //LOG("Submod instance %p\n", inst_b);

          if (inst_submod && inst_b) {
            bool ports_ok = inst_submod->check_port_directions(inst_b);
            if (!ports_ok) {
              LOG_R("-----\n");
              inst_submod->dump_tree();
              LOG_R("-----\n");
              inst_b->dump_tree();
              LOG_R("-----\n");
              err << ERR("Bad ports!\n");
              exit(-1);
            }
          }
        }
      }
    }

    LOG("\n");
  }

  //----------------------------------------

  LOG_R("//----------------------------------------\n");
  LOG_R("// Call graph\n");
  LOG("\n");

  for (auto node_class : repo.all_classes) {
    node_class->dump_call_graph();
    LOG("\n");
    node_class->dump();
    LOG("\n");
  }

  LOG_R("//----------------------------------------\n");
  LOG("\n");

  for (auto i : repo.all_instances) {
    i->dump_tree();
  }

  LOG_R("//----------------------------------------\n");
  LOG("\n");

  LOG_R("early exit 2!\n");
  exit(0);

  //----------------------------------------

  LOG_B("Categorizing fields\n");

  /*
  Err err;

  if (verbose) {
    LOG_G("Categorizing %s\n", name().c_str());
  }

  for (auto f : all_fields) {
    if (f->is_param()) {
      continue;
    }

    if (f->is_component()) {
      components.push_back(f);
    }
    else if (f->is_public() && f->is_input()) {
      input_signals.push_back(f);
    }
    else if (f->is_public() && f->is_signal()) {
      output_signals.push_back(f);
    }
    else if (f->is_public() && f->is_register()) {
      output_registers.push_back(f);
    }
    else if (f->is_private() && f->is_register()) {
      private_registers.push_back(f);
    }
    else if (f->is_private() && f->is_signal()) {
      private_signals.push_back(f);
    }
    else if (!f->is_public() && f->is_input()) {
      private_registers.push_back(f);
    }
    else if (f->is_enum()) {
    }
    else if (f->is_dead()) {
      dead_fields.push_back(f);
    }
    else {
      err << ERR("Don't know how to categorize %s = %s\n", f->cname(),
                 to_string(f->_state));
      f->error();
    }
  }
  */



  for (auto node_class : repo.all_classes) {

    LOG_INDENT_SCOPE();
    auto name = node_class->get_namestr();
    LOG_G("Categorizing %s\n", name.c_str());

    for (auto f : node_class->all_fields) {
      auto fname = f->get_namestr();

      if (!f->is_public) continue;

#if 0
      if (f->field_type == FT_INPUT) {
        LOG_G("input signal %s\n", fname.c_str());
        node_class->input_signals.push_back(f);
      }
      else if (f->field_type == FT_OUTPUT || f->field_type == FT_SIGNAL) {
        LOG_G("output signal %s\n", fname.c_str());
        node_class->output_signals.push_back(f);
      }
      else if (f->field_type == FT_REGISTER) {
        LOG_G("output register f%s\n", fname.c_str());
        node_class->output_registers.push_back(f);
      }
#endif
    }
  }


#if 0
  for (auto c : repo.all_classes) {
    for (auto f : c->all_fields) {
      if (f->node_decl->_type_class) {
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

#endif






  LOG_B("//----------------------------------------\n");
  LOG_B("// Class dump\n");

  for (auto c : repo.all_classes) {
    c->dump();
    LOG("\n");
  }


  LOG_R("early exit\n");
  LOG("");
  exit(0);


  //========================================

  LOG_B("Categorizing methods\n");

  //----------------------------------------
  // Everything called by a constructor is init.

  for (auto c : repo.all_classes) {
    if (auto f = c->constructor) {
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
      // FIXME in init_chain, why wasn't the transitive write showing up in init1?
      // because we're not tracing constructors yet
      if (f->method_type == MT_UNKNOWN) {
        if (f->all_writes.empty() && f->external_callees.empty()) f->set_type(MT_FUNC);
      }
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
  // Methods that directly read or write arrays _must_ be ticks.

#if 0
  for (auto c : repo.all_classes) {
    for (auto f : c->all_functions) {
      for (auto r : f->self_reads)  if (r->is_array()) f->set_type(MT_TICK);
      for (auto w : f->self_writes) if (w->is_array()) f->set_type(MT_TICK);
    }
  }
#endif

  //----------------------------------------
  // Methods that directly write registers _must_ be ticks.

#if 0
  for (auto c : repo.all_classes) {
    for (auto f : c->all_functions) {
      for (auto w : f->self_writes) if (w->field_type == FT_REGISTER) f->set_type(MT_TICK);
    }
  }
#endif

  //----------------------------------------
  // Everything else that's unassigned can be a tock? Sure?

  for (auto c : repo.all_classes) {
    for (auto f : c->all_functions) {
      if (f->method_type == MT_UNKNOWN) f->set_type(MT_TOCK);
    }
  }

  //----------------------------------------

#if 0
  for (auto c : repo.all_classes) {
    for (auto f : c->all_functions) {
      if (f->method_type == MT_INIT) continue;

      bool self_writes_sig = false;
      bool self_writes_reg = false;
      bool all_writes_sig = false;
      bool all_writes_reg = false;
      bool all_writes_out = false;

      for (auto w : f->self_writes) {
        if (w->field_type == FT_SIGNAL)   self_writes_sig = true;
        if (w->field_type == FT_REGISTER) self_writes_reg = true;
      }

      for (auto w : f->all_writes) {
        if (w->field_type == FT_SIGNAL)   all_writes_sig = true;
        if (w->field_type == FT_REGISTER) all_writes_reg = true;
        if (w->field_type == FT_OUTPUT)   all_writes_out = true;
      }

      if (self_writes_sig && self_writes_reg) {
        // Methods that directly write both signals and registers are invalid.
        assert(false);
      }
      else if (self_writes_reg) {
        f->set_type(MT_TICK);
      }
      else if (all_writes_reg || all_writes_sig || all_writes_out) {
        f->set_type(MT_TOCK);
      }
      else {
        f->set_type(MT_FUNC);
      }
    }
  }
#endif

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

#if 0
  for (auto c : repo.all_classes) {
    for (auto f : c->all_fields) {
      assert(f->field_type != FT_UNKNOWN);
    }
    for (auto f : c->all_functions) {
      if (f->method_type == MT_UNKNOWN) {
        auto name = f->get_name();
        LOG_R("%.*s did not get a type\n", name.size(), name.data());
      }
      assert(f->method_type != MT_UNKNOWN);
    }
  }
#endif

  //----------------------------------------

  /*if (opts.verbose)*/ {
    LOG_B("\n");
    LOG_B("//----------------------------------------\n");
    LOG_B("// Repo dump\n\n");
    LOG_INDENT_SCOPE();
    repo.dump();
    LOG_B("//----------------------------------------\n");
    LOG_B("\n");
  }

  //exit(0);


  //----------------------------------------
  // Emit all modules.

  LOG_G("Converting %s to SystemVerilog\n", opts.src_name.c_str());

  std::string out_string;
  Cursor cursor(&repo, root_file, &out_string);
  cursor.echo = opts.echo && !opts.quiet;

  if (opts.echo) LOG_G("\n----------------------------------------\n");
  err << cursor.emit_everything();
  //err << cursor.emit_trailing_whitespace();
  //err << cursor.emit_gap();
  if (opts.echo) LOG_G("----------------------------------------\n\n");

  if (err.has_err()) {
    LOG_R("Error during code generation\n");
    //lib.teardown();
    return -1;
  }

  //----------------------------------------

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

  LOG_B("================================================================================\n\n");
  LOG_G("%s\n", out_string.c_str());

  //----------------------------------------

  LOG("Done!\n");

  return 0;
}
