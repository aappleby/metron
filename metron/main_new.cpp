#include "main.h"

#include "matcheroni/Utilities.hpp"
#include "metrolib/core/Log.h"
#include "metrolib/core/Platform.h"
#include "metron/CInstance.hpp"
#include "metron/CNodeClass.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/CNodeFunction.hpp"
#include "metron/CNodeStruct.hpp"
#include "metron/CParser.hpp"
#include "metron/CSourceFile.hpp"
#include "metron/CSourceRepo.hpp"
#include "metron/Cursor.hpp"
#include "metron/NodeTypes.hpp"
#include "metron/Tracer.hpp"
#include "metron/MtUtils.h"

using namespace matcheroni;

bool deep_trace = false;
bool writes_are_bad = false;

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

  root_file->context.top_head->dump_parse_tree();
  LOG("\n");

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
  // All modules are now in the library, we can resolve references to other
  // modules when we're collecting fields.

  LOG_B("Processing source files\n");

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

  LOG_B("Collecting fields and methods\n");

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

  // Count module instances so we can find top modules.

  for (auto c : repo.all_classes) {
    for (auto f : c->all_fields) {
      if (f->node_decl->_type_class) f->node_decl->_type_class->refcount++;
    }
  }

  CNodeClass* top = nullptr;
  for (auto c : repo.all_classes) {
    if (c->refcount == 0) {
      assert(top == nullptr);
      top = c;
    }
  }
  //assert(top);

  CInstClass* top_inst = nullptr;

  if (top) {
    top_inst = instantiate_class(top->get_namestr(), nullptr, nullptr, top, 1000);
    LOG_INDENT();
    LOG_G("Top instance is %s\n", top->get_namestr().c_str());
    LOG_DEDENT();
  }

  //----------------------------------------

  LOG_B("Building call graph\n");
  for (auto node_class : repo.all_classes) {
    err << node_class->build_call_graph(&repo);
  }

  for (auto node_class : repo.all_classes) {
    for (auto node_func : node_class->all_functions) {

      if (node_func->method_type != MT_FUNC && node_func->is_public && node_func->internal_callers.size()) {
        LOG_R("Function %s is public and has internal callers\n", node_func->name.c_str());
        assert(false);
      }
      //err << node_class->build_call_graph(&repo);
    }
  }

  //----------------------------------------

  LOG_B("Instantiating modules\n");
  for (auto node_class : repo.all_classes) {
    auto instance = instantiate_class(node_class->get_namestr(), nullptr, nullptr, node_class, 1000);
    repo.all_instances.push_back(instance);
  }

  //----------------------------------------
  // Top methods that are not ticks or tocks must not write anything

  // Collect all the writes for all functions but without tracing :/
  {
    bool any_writes = false;
    CNode* written_field = nullptr;
    CNodeFunction* current_func = nullptr;

    node_visitor gather_writes = [&](CNode* node) {
      if (any_writes) return;
      if (auto node_assignment = node->as<CNodeAssignment>()) {
          auto lhs = node_assignment->child("lhs")->req<CNodeLValue>();
          auto field = node->ancestor<CNodeClass>()->get_field(lhs);
          if (field) {
            current_func->self_writes2.insert(field);
          }
      }
    };

    for (auto node_class : repo.all_classes) {
      for (auto node_func : node_class->all_functions) {
        current_func = node_func;
        visit(node_func, gather_writes);
      }
    }

    for (auto node_class : repo.all_classes) {
      for (auto node_func : node_class->all_functions) {
        node_func->collect_writes2();
      }
    }


    for (auto node_class : repo.all_classes) {
      for (auto node_func : node_class->all_functions) {
        LOG_R("Func %s\n", node_func->get_namestr().c_str());
        LOG_INDENT();
        for (auto node_field : node_func->self_writes2) {
          LOG_R("Writes direct %s\n", node_field->get_namestr().c_str());
        }
        for (auto node_field : node_func->all_writes2) {
          if (node_func->self_writes2.contains(node_field)) {
          }
          else {
            LOG_R("Writes indirect %s\n", node_field->get_namestr().c_str());
          }
        }
        for (auto node_callee : node_func->internal_callees) {
          LOG_R("Calls internal %s\n", node_callee->get_namestr().c_str());
        }
        for (auto node_callee : node_func->external_callees) {
          LOG_R("Calls external %s\n", node_callee->get_namestr().c_str());
        }

        LOG_DEDENT();
      }
    }

    for (auto node_class : repo.all_classes) {
      for (auto node_func : node_class->all_functions) {
        auto func_name = node_func->get_namestr();
        if (!node_func->is_public) continue;
        if (func_name.starts_with("tick")) continue;
        if (func_name.starts_with("tock")) continue;

        if (node_func->all_writes2.size()) {
          LOG_R("Top-level func %s writes stuff and it shouldn't\n", func_name.c_str());
          exit(-1);
        }
      }
    }
  }

  //----------------------------------------
  // Check for ticks with return values

  for (auto node_class : repo.all_classes) {
    for (auto node_func : node_class->all_functions) {
      auto func_name = node_func->get_namestr();
      if (func_name.starts_with("tick")) {
        if (node_func->has_return()) {
          LOG_R("Tick %s has a return value and it shouldn't\n", func_name.c_str());
          exit(-1);
        }
      }
    }
  }

  //----------------------------------------
  // Trace

  LOG_B("Tracing classes\n");
  LOG_INDENT();

  for (auto inst_class : repo.all_instances) {
    auto node_class = inst_class->node_class;
    auto name = node_class->get_name();
    LOG_B("Tracing public methods in %.*s\n", int(name.size()), name.data());

    // Trace constructors first
    if (auto node_func = node_class->constructor) {
      LOG_INDENT_SCOPE();
      auto func_name = node_func->get_namestr();
      //if (!node_func->is_public) continue;

      LOG_B("Tracing %s\n", func_name.c_str());
      auto inst_func = inst_class->resolve(func_name);
      call_stack stack;
      stack.push_back(node_func);
      err << node_func->trace(inst_func, stack);
    }

    // Trace tocks
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

    // Trace ticks
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

    // Trace top functions
    /*
    for (auto node_func : node_class->all_functions) {
      LOG_INDENT_SCOPE();
      auto func_name = node_func->get_namestr();
      if (func_name.starts_with("tick")) continue;
      if (func_name.starts_with("tock")) continue;
      if (!node_func->is_public) continue;

      LOG_B("Tracing %s\n", func_name.c_str());
      auto inst_func = inst_class->resolve(func_name);
      call_stack stack;
      stack.push_back(node_func);

      writes_are_bad = true;
      err << node_func->trace(inst_func, stack);
      writes_are_bad = false;
    }
    */
  }

  for (auto c : repo.all_classes) {
    if (c->constructor) {
      c->constructor->propagate_rw();
    }
    for (auto f : c->all_functions) {
      f->propagate_rw();
    }
  }

  if (err.has_err()) {
    LOG_R("Error during tracing\n");
    return -1;
  }

  LOG_DEDENT();

  //----------------------------------------

  LOG_B("Tracing top module\n");
  LOG_INDENT();

  deep_trace = true;

  if (top_inst) {
    // Trace constructors first
    if (auto node_func = top->constructor) {
      LOG_INDENT_SCOPE();
      auto func_name = node_func->get_namestr();

      LOG_B("Tracing %s\n", func_name.c_str());
      auto inst_func = top_inst->resolve(func_name);
      call_stack stack;
      stack.push_back(node_func);
      err << node_func->trace(inst_func, stack);
    }

    // Trace tocks
    for (auto node_func : top->all_functions) {
      LOG_INDENT_SCOPE();
      auto func_name = node_func->get_namestr();
      if (!func_name.starts_with("tock")) continue;
      if (!node_func->is_public) continue;

      LOG_B("Tracing %s\n", func_name.c_str());
      auto inst_func = top_inst->resolve(func_name);
      call_stack stack;
      stack.push_back(node_func);
      err << node_func->trace(inst_func, stack);
    }

    // Trace ticks
    for (auto node_func : top->all_functions) {
      LOG_INDENT_SCOPE();
      auto func_name = node_func->get_namestr();
      if (!func_name.starts_with("tick")) continue;
      if (!node_func->is_public) continue;

      LOG_B("Tracing %s\n", func_name.c_str());
      auto inst_func = top_inst->resolve(func_name);
      call_stack stack;
      stack.push_back(node_func);
      err << node_func->trace(inst_func, stack);
    }
  }

  deep_trace = false;

  if (top_inst) {
    top_inst->dump_tree();
  }

  if (err.has_err()) {
    LOG_R("Error during tracing\n");
    return -1;
  }
  LOG_DEDENT();

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

      if (method_type == MT_FUNC) {
        assert(f->all_writes.empty());
      }
      else if (method_type == MT_TOCK) {
        for (auto inst : f->self_writes) {
          auto state = inst->get_state();
          if (state == TS_SIGNAL || state == TS_OUTPUT) {
          }
          else {
            LOG_R("Tock writes a non-signal\n");
            LOG("");
            exit(-1);
          }
        }
      }
      else if (method_type == MT_TICK) {
        for (auto inst : f->self_writes) {
          auto state = inst->get_state();
          if (state == TS_REGISTER || state == TS_MAYBE || state == TS_OUTPUT) {
          }
          else {
            LOG_R("Tick writes a non-register\n");
            LOG("");
            exit(-1);
          }
        }
      }
    }
  }

  //----------------------------------------

  LOG_B("Checking port compatibility\n");
  LOG_INDENT();

  for (auto inst : repo.all_instances) {
    LOG("Module %s\n", inst->name.c_str());
    LOG_INDENT_SCOPE();
    for (auto child : inst->parts) {
      if (auto inst_a = child->as<CInstClass>()) {
        auto inst_b = repo.get_instance(inst_a->node_class->get_namestr());
        assert(inst_b);
        bool ports_ok = inst_a->check_port_directions(inst_b);
        if (!ports_ok) {
          LOG_R("-----\n");
          inst_a->dump_tree();
          LOG_R("-----\n");
          inst_b->dump_tree();
          LOG_R("-----\n");
          err << ERR("Bad ports!\n");
          exit(-1);
        }
      }
    }
  }
  LOG_DEDENT();

  //----------------------------------------

  LOG_B("Categorizing fields\n");

  for (auto inst : repo.all_instances) {
    auto inst_class = inst->as<CInstClass>();
    auto node_class = inst_class->node_class;
    assert(inst_class);

    for (auto inst_child : inst_class->ports) {

      if (auto inst_prim = inst_child->as<CInstPrim>()) {
        auto field = inst_prim->node_field;
        if (!field->is_public) continue;
        if (field->node_decl->node_static && field->node_decl->node_const) continue;

        switch(inst_prim->get_state()) {
          case TS_NONE:     break;
          case TS_INPUT:    node_class->input_signals.push_back(field);    break;
          case TS_OUTPUT:   node_class->output_signals.push_back(field);   break;
          case TS_MAYBE:    node_class->output_registers.push_back(field); break;
          case TS_SIGNAL:   node_class->output_signals.push_back(field);   break;
          case TS_REGISTER: node_class->output_registers.push_back(field); break;
          case TS_INVALID:  assert(false); break;
          case TS_PENDING:  assert(false); break;
        }
      }

      if (auto inst_struct = inst_child->as<CInstStruct>()) {
        auto field = inst_struct->node_field;
        if (!field->is_public) continue;
        if (field->node_decl->node_static && field->node_decl->node_const) continue;

        switch(inst_struct->get_state()) {
          case TS_NONE:     break;
          case TS_INPUT:    node_class->input_signals.push_back(field);    break;
          case TS_OUTPUT:   node_class->output_signals.push_back(field);   break;
          case TS_MAYBE:    node_class->output_registers.push_back(field); break;
          case TS_SIGNAL:   node_class->output_signals.push_back(field);   break;
          case TS_REGISTER: node_class->output_registers.push_back(field); break;
          case TS_INVALID:  assert(false); break;
          case TS_PENDING:  assert(false); break;
        }
      }
    }
  }

#if 0
  for (auto node_class : repo.all_classes) {

    LOG_INDENT_SCOPE();
    auto name = node_class->get_namestr();
    LOG_G("Categorizing %s\n", name.c_str());

    for (auto f : node_class->all_fields) {
      auto fname = f->get_namestr();

      if (!f->is_public) continue;

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
    }
  }
#endif


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

  //========================================

  LOG_B("Categorizing methods\n");

#if 0
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

  LOG_B("\n");
  LOG_B("================================================================================\n");
  LOG_B("Repo dump\n\n");

  repo.dump();

  for (auto inst_class : repo.all_instances) {
    LOG_B("Instance tree for %s\n", inst_class->node_class->get_namestr().c_str());
    LOG_INDENT();
    inst_class->dump_tree();
    LOG_DEDENT();
    LOG_B("\n");
  }

  for (auto node_class : repo.all_classes) {
    LOG_B("Call graph for %s\n", node_class->get_namestr().c_str());
    LOG_INDENT();
    node_class->dump_call_graph();
    LOG_DEDENT();
    LOG("\n");
  }


  LOG_B("================================================================================\n");
  LOG_B("Converting %s to SystemVerilog\n\n", opts.src_name.c_str());

  std::string out_string;
  Cursor cursor(&repo, root_file, &out_string);
  cursor.echo = opts.echo && !opts.quiet;

  err << cursor.emit_everything();
  //err << cursor.emit_trailing_whitespace();
  //err << cursor.emit_gap();

  if (err.has_err()) {
    LOG_R("Error during code generation\n");
    //lib.teardown();
    return -1;
  }

  // Save translated source to output directory, if there is one.
  if (opts.dst_name.size()) {

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
  LOG("\n");

  LOG_B("================================================================================\n");
  LOG_B("Output:\n\n");

  LOG_G("%s\n", out_string.c_str());

  LOG_B("================================================================================\n");

  LOG("Done!\n");

  return 0;
}
