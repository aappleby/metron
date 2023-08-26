#include "Tracer.hpp"

#include "CSourceRepo.hpp"
#include "CInstance.hpp"

#include "CNodeClass.hpp"
#include "CNodeExpression.hpp"
#include "CNodeFunction.hpp"
#include "CNodeStatement.hpp"
#include "CNodeType.hpp"
#include "NodeTypes.hpp"

#if 0

//------------------------------------------------------------------------------

Tracer::Tracer(CSourceRepo* repo, bool verbose)
: repo(repo), verbose(verbose) {
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::log_action(CInstCall* call, CNode* node, CInstField* field_inst, TraceAction action) {
  Err err;

  err << field_inst->log_action(node, action);

  /*
  if (action == ACT_WRITE) {
    call->_method_inst->_writes.insert(target);
    call->_writes.insert(target);
  }

  if (action == ACT_READ) {
    call->_method_inst->_reads.insert(target);
    call->_reads.insert(target);
  }
  */

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace() {
  Err err;
  LOG_INDENT_SCOPE();

  auto root_inst = new CInstClass(nullptr, repo->top);
  auto root_name = repo->top->get_name();

  std::vector<CInstCall*> calls;

  for (auto inst_func : root_inst->inst_functions) {
    auto node_func = inst_func->node_function;
    //node_func->dump_tree();

    if (node_func->internal_callers.size()) continue;

    auto func_name = node_func->get_name();

    LOG_B("Tracing %.*s::%.*s\n",
      int(root_name.size()), root_name.data(),
      int(func_name.size()), func_name.data()
    );

    auto inst_call = new CInstCall(nullptr, nullptr);

    for (auto inst_arg : inst_call->inst_args) {
      err << inst_arg->log_action(nullptr, ACT_WRITE);
    }

    /*
    for(auto node_param : node_func->child("params")) {
      auto inst_arg = new CInstArg();
      inst_arg->node_param = node_param;

      err << inst_arg->log_action(nullptr, ACT_WRITE);

      inst_call->inst_args.push_back(inst_arg);
    }
    inst_call->inst_return = new CInstReturn();
    inst_call->inst_return->node_return = node_func->child_is<CNodeType>("return_type");
    */

    calls.push_back(inst_call);
    LOG_INDENT();
    //err << trace_top_call(inst_call);
    //err << inst_call->trace();
    err << inst_call->trace(ACT_READ);
    LOG_DEDENT();

    LOG_B("Tracing %.*s::%.*s done\n",
      int(root_name.size()), root_name.data(),
      int(func_name.size()), func_name.data()
    );
  }

  LOG_B("Calls:\n");
  LOG_INDENT();
  for (auto c : calls) c->dump_tree();
  LOG_DEDENT();

#if 0
  err << root_inst->assign_types();
  //err << root_inst->sanity_check();
  root_inst->dump();
#endif

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_top_call(CInstCall* call) {
  Err err;

  /*
  auto node_body = call->_method->_node.get_field(field_body);
  if (MtChecker::has_non_terminal_return(node_body)) {
    err << ERR("Method %s has non-terminal return\n", call->_method->cname());
  }

  err << trace_function_definition(call, call->_method->_node);
  */

  //err << trace_function_definition(call, call->node_function);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_identifier(CInstCall* call, CNode* node, TraceAction action) {
  Err err;

  if (auto id = node->as_a<CNodeIdentifier>()) {
  }

  /*
  switch (node.sym) {
    case sym_qualified_identifier:
    case alias_sym_namespace_identifier:
      // pretty sure these can't do anything
      break;
    case sym_identifier:
    case alias_sym_field_identifier: {
      if (auto field_inst = call->_method_inst->_module->get_field(node.text())) {
        err << log_action(call, node, field_inst, action);
        break;
      }

      / *
      if (auto param_inst = call->get_param(node.text())) {
        err << log_action(call, node, param_inst, action);
        break;
      }
      * /

      if (call->_method_inst->has_local(node.text())) {
        //LOG_B("Identifier %s is a local\n", node.text().c_str());
      }
      else {
        // FIXME can't currently resolve constants from namespaces
        //LOG_R("Could not resolve identifier %s\n", node.text().c_str());
      }
      break;
    }
    default:
      err << trace_default(call, node);
      break;
  }
  */

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_declarator(CInstCall* call, CNode* node) {
  Err err;

  /*
  switch (node.sym) {
    case sym_identifier:
      err << trace_identifier(call, node, ACT_READ);
      break;
    case sym_init_declarator:
      err << trace_init_declarator(call, node);
      break;
    default:
      err << trace_default(call, node);
      break;
  }
  */

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_statement(CInstCall* call, CNode* node) {
  Err err;

  /*
  switch (node.sym) {
    case sym_compound_statement:
      err << trace_compound_statement(call, node);
      break;
    case sym_case_statement:
      err << trace_case_statement(call, node);
      break;
    case sym_if_statement:
      err << trace_if_statement(call, node);
      break;
    case sym_expression_statement:
      err << trace_expression(call, node.child(0), ACT_READ);
      break;
    case sym_switch_statement:
      err << trace_switch_statement(call, node);
      break;
    case sym_return_statement:
      err << trace_return_statement(call, node);
      break;
    case sym_for_statement:
      err << trace_for_statement(call, node);
      break;

    default:
      err << trace_default(call, node);
      break;
  }
  */

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_expression(CInstCall* call, CNodeExpression* node, TraceAction action) {
  Err err;

  /*
  switch (node.sym) {
    case sym_identifier:
    case sym_qualified_identifier:
      err << trace_identifier(call, node, action);
      break;
    case sym_conditional_expression:
      err << trace_conditional_expression(call, node);
      break;
    case sym_field_expression:
      err << trace_field_expression(call, node, action);
      break;
    case sym_subscript_expression:
      err << trace_expression(call, node.get_field(field_index), ACT_READ);
      err << trace_expression(call, node.get_field(field_argument), action);
      break;
    case sym_call_expression:
      err << trace_call_expression(call, node);
      break;
    case sym_update_expression:
      // this is "i++" or similar, which is a read and a write.
      err << trace_expression(call, node.get_field(field_argument), ACT_READ);
      err << trace_expression(call, node.get_field(field_argument), ACT_WRITE);
      break;
    case sym_assignment_expression:
      err << trace_assignment_expression(call, node);
      break;
    case sym_parenthesized_expression:
      err << trace_expression(call, node.child(1), action);
      break;

    case sym_unary_expression:
      err << trace_expression(call, node.get_field(field_argument), ACT_READ);
      break;

    case sym_binary_expression:
      err << trace_binary_expression(call, node);
      break;

    case sym_initializer_list:
      err << trace_initializer_list(call, node);
      break;

    default:
      err << trace_default(call, node);
      break;
  }
  */

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_call(CInstCall* call, CInstFunction* method, CNode* call_node) {
  Err err;

#if 0
  if (!method) {
    // This is a call to b32() or something similar. We should eventually check
    // that this is a known helper function, but for now we can ignore it.
    //node_call.dump_tree();
    //return ERR("dst_inst is null");
    return err;
  }

  if (!call) return ERR("parent call is null");

  // If the source and dest functions are not in the same module and the source
  // module has to pass params to the dest module, we have to bind the params
  // to ports to "call" it.

  call->_method_inst->_call_methods.insert(method);
  method->_called_by.insert(call->_method_inst);

  bool cross_mod_call = call->_method_inst->_module != method->_module;

  auto new_call = new MtCallInstance(
    method->_name,
    call->_path + "." + call->_module->name() + "::" + method->_name,
    call,
    call_node,
    method
  );

  call->_method_inst->_calls.push_back(new_call);

  call->_calls.push_back(new_call);

  // FIXME - this should also catch calling tick() multiple times inside a single module

  // This doesn't work because it destroys state inside branches
  /*
  if (!cross_mod_call) {
    dst_inst->reset_state();
  }
  */

  /*
  for (auto p : new_call->_params) {
    err << log_action(call, call_node, p, ACT_WRITE);
  }
  */


  if (cross_mod_call) {

    /*
    for (auto param : dst_inst->_params) {
      //LOG_R("Write %s.%s.%s\n", child_name.c_str(), child_func.c_str(), param.first.c_str());
      err << log_action(src_inst, node_call, param, ACT_WRITE);
    }
    */
  }

  /*
  for (auto param : dst_inst->_params) {
    //LOG_R("Write %s.%s.%s\n", child_name.c_str(), child_func.c_str(), param.first.c_str());
    err << log_action(src_inst, node_call, param, ACT_WRITE);
  }
  */

  err << trace_function_definition(new_call, method->_method->_node);

  /*
  if (new_call->_retval) {
    err << new_call->_retval->log_action(call_node, ACT_READ);
  }
  */

  //err << dst_inst->_retval->log_action(node_call, ACT_READ);
#endif

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_default(CInstCall* call, CNode* node) {
  Err err;

#if 0
  if (!node.is_named()) return err;

  switch (node.sym) {
    case sym_comment:
    case sym_using_declaration:
    case sym_number_literal:
    case sym_string_literal:
    case sym_break_statement:
      break;
    default:
      // KCOV_OFF
      err << ERR("%s : No handler for %s\n", __func__, node.ts_node_type());
      node.error();
      break;
      // KCOV_ON
  }
#endif

  return err;
}




//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



CHECK_RETURN Err Tracer::trace_argument_list(CInstCall* call, CNode* node) {
  Err err;

#if 0
  assert(node.sym == sym_argument_list);

  for (const auto& child : node) {
    if (child.is_identifier()) {
      err << trace_identifier(call, child, ACT_READ);
    } else if (child.is_expression()) {
      err << trace_expression(call, child, ACT_READ);
    } else {
      err << trace_default(call, child);
    }
  }
#endif

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_assignment_expression(CInstCall * call, CNode* node) {
  Err err;

#if 0
  auto op = node.get_field(field_operator).text();

  if (op == "=") {
    err << trace_expression(call, node.get_field(field_right), ACT_READ);
    err << trace_expression(call, node.get_field(field_left), ACT_WRITE);
  }
  else {
    err << trace_expression(call, node.get_field(field_right), ACT_READ);
    err << trace_expression(call, node.get_field(field_left), ACT_READ);
    err << trace_expression(call, node.get_field(field_left), ACT_WRITE);
  }
#endif

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_binary_expression(CInstCall* call, CNodeExpression* node) {
  Err err;
  err << trace_expression(call, node->child("lhs")->is_a<CNodeExpression>(), ACT_READ);
  err << trace_expression(call, node->child("rhs")->is_a<CNodeExpression>(), ACT_READ);
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_call_expression(CInstCall* call, CNode* node) {
  Err err;

#if 0
  assert(node.sym == sym_call_expression);

  auto node_func = node.get_field(field_function);
  auto node_args = node.get_field(field_arguments);

  // Trace the args first.
  err << trace_argument_list(call, node_args);

  // Now trace the actual call.
  switch (node_func.sym) {
    case sym_field_expression: {
      auto child_name = node_func.get_field(field_argument).name4();
      auto child_func = node_func.get_field(field_field).name4();
      auto child_inst = call->_method_inst->_module->get_field(child_name);
      auto child_func_inst = child_inst->as_a<MtModuleInstance>()->get_method(child_func);
      err << trace_call(call, child_func_inst, node);
      break;
    }
    case sym_identifier:
      err << trace_call(call, call->_method_inst->_module->get_method(node_func.text()), node);
      break;

    case sym_template_function: {
      // FIXME this is a stub, we don't currently have real template function
      // support
      auto node_name =
          node.get_field(field_function).get_field(field_name).text();
      if (node_name == "bx" ||
          node_name == "dup" ||
          node_name == "sign_extend" ||
          node_name == "zero_extend") {
      } else {
        err << ERR("trace_call_expression - Unhandled template func %s\n", node.text().c_str());
        return err;
      }
      break;
    }

    default:
      // KCOV_OFF
      err << ERR("%s : No handler for %s\n", __func__, node.ts_node_type());
      node.error();
      break;
      // KCOV_ON
  }
#endif

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_case_statement(CInstCall* call, CNode* node) {
  Err err;

#if 0
  assert(node.sym == sym_case_statement);

  // Everything after the colon should be statements.

  bool hit_colon = false;
  for (auto child : node) {
    if (child.sym == anon_sym_COLON) {
      hit_colon = true;
      continue;
    }
    if (hit_colon) {
      err << trace_statement(call, child);
    }
  }

  if (!MtChecker::ends_with_break(node)) {
    err << ERR("Case statement in %s does not end with break\n", call->_name.c_str());
  }
#endif

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_compound_statement(CInstCall* call, CNodeCompound* node) {
  Err err;

  node->dump_tree(1);

  for (auto n : node) {
  }

#if 0
  assert(node.sym == sym_compound_statement);

  bool noconvert = false;
  bool dumpit = false;

  for (const auto& child : node) {
    if (noconvert) { noconvert = false; continue; }
    if (dumpit) { child.dump_tree(); dumpit = false; }
    if (child.sym == sym_comment && child.contains("dumpit")) { dumpit = true; }
    if (child.sym == sym_comment && child.contains("metron_noconvert")) { noconvert = true; }

    if (child.sym == sym_declaration) {
      err << trace_declaration(call, child);
    } else if (child.is_statement()) {
      err << trace_statement(call, child);
    } else {
      err << trace_default(call, child);
    }
  }
#endif

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_condition_clause(CInstCall* call, CNode* node) {
  Err err;
#if 0
  assert(node.sym == sym_condition_clause);

  auto node_value = node.get_field(field_value);
  err << trace_expression(call, node_value, ACT_READ);
#endif

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_conditional_expression(CInstCall* call, CNode* node) {
  Err err;
  #if 0
  assert(node.sym == sym_conditional_expression);

  auto node_cond = node.get_field(field_condition);
  auto node_branch_a = node.get_field(field_consequence);
  auto node_branch_b = node.get_field(field_alternative);

  err << trace_expression(call, node_cond, ACT_READ);

  call->_method_inst->_module->visit([=](MtInstance* m) { m->push_state(node_branch_a); });
  if (!node_branch_a.is_null()) {
    err << trace_expression(call, node_branch_a, ACT_READ);
  }
  call->_method_inst->_module->visit([=](MtInstance* m) { m->swap_state(node_branch_b); });
  if (!node_branch_b.is_null()) {
    err << trace_expression(call, node_branch_b, ACT_READ);
  }
  call->_method_inst->_module->visit([=](MtInstance* m) { m->merge_state(MnNode::null); });
  #endif

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_declaration(CInstCall* call, CNode* node) {
  Err err;
  #if 0
  assert(node.sym == sym_declaration);

  call->_method_inst->_scope_stack.back().insert(node.name4());

  auto node_type = node.get_field(field_type);
  auto node_decl = node.get_field(field_declarator);

  err << trace_declarator(call, node_decl);
  #endif

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_field_expression(CInstCall* call, CNode* node, TraceAction action) {
  Err err;
  #if 0
  assert(node.sym == sym_field_expression);


  auto path = split_field(node.text());

  auto f = call->_module_inst->get_field(path[0]);
  auto p = call->get_param(path[0]);

  MtInstance* r = nullptr;
  if (f) {
    r = f->resolve(path, 1);
  }
  /*
  if (p) {
    r = p->resolve(path, 1);
  }
  */

  if (r) {
    err << log_action(call, node, r, action);
  }
  else {
    // FIXME
    LOG_R("Not resolved\n");
    node.dump_tree();
    //return ERR("Not resolved\n");
  }
  #endif

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_for_statement(CInstCall* call, CNode* node) {
  Err err;

#if 0
  for (auto c : node) {
    if (c.field == field_initializer) {
      if (c.sym == sym_declaration) {
        err << trace_declaration(call, c);
      }
      else {
        err << trace_expression(call, c, ACT_READ);
      }
    }
    else if (c.is_expression()) {
      err << trace_expression(call, c, ACT_READ);
    }
    else if (c.is_statement()) {
      err << trace_statement(call, c);
    }
    else {
      err << trace_default(call, c);
    }
  }
#endif

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_function_definition(CInstCall* call, CNode* node) {
  Err err;

  auto func = call->node_function;
  auto body = func->child_is<CNodeCompound>("body");

  //call->node_function->dump_tree(2);

  #if 0
  assert(node.sym == sym_function_definition);

  auto node_type = node.get_field(field_type);
  auto node_decl = node.get_field(field_declarator);
  auto node_body = node.get_field(field_body);

  #endif
  err << trace_compound_statement(call, body);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_if_statement(CInstCall* call, CNode* node) {
  Err err;
  #if 0
  assert(node.sym == sym_if_statement);

  //node.dump_tree();

  auto node_cond = node.get_field(field_condition);
  auto node_branch_a = node.get_field(field_consequence);
  auto node_branch_b = node.get_field(field_alternative);

  err << trace_condition_clause(call, node_cond);

  call->_module_inst->visit([=](MtInstance* m) { m->push_state(node_cond); });
  if (!node_branch_a.is_null()) {
    err << trace_statement(call, node_branch_a);
  }
  call->_module_inst->visit([=](MtInstance* m) { m->swap_state(MnNode::null); });
  if (!node_branch_b.is_null()) {
    err << trace_statement(call, node_branch_b);
  }
  call->_module_inst->visit([=](MtInstance* m) { m->merge_state(MnNode::null); });
  #endif

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_init_declarator(CInstCall* call, CNode* node) {
  Err err;
  #if 0
  assert(node.sym == sym_init_declarator);

  err << trace_expression(call, node.get_field(field_value), ACT_READ);
  #endif

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_initializer_list(CInstCall* call, CNode* node) {
  Err err;
  #if 0
  assert(node.sym == sym_initializer_list);

  for (const auto& child : node) {
    if (child.is_identifier()) {
      err << trace_identifier(call, child, ACT_READ);
    } else if (child.is_expression()) {
      err << trace_expression(call, child, ACT_READ);
    } else {
      err << trace_default(call, child);
    }
  }
  #endif

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_return_statement(CInstCall* call, CNode* node) {
  Err err;
#if 0
  assert(node.sym == sym_return_statement);

  auto node_lit = node.child(0);
  auto node_value = node.child(1);
  auto node_semi = node.child(2);

  err << trace_expression(call, node_value, ACT_READ);

  /*
  if (call->_retval) {
    err << call->_retval->log_action(node, ACT_WRITE);
  }
  */
#endif

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err Tracer::trace_switch_statement(CInstCall* call, CNode* node) {
  Err err;
#if 0
  assert(node.sym == sym_switch_statement);

  err << trace_condition_clause(call, node.get_field(field_condition));

  auto body = node.get_field(field_body);

  bool has_default = false;
  int case_count = 0;

  for (const auto& child : body) {
    if (child.sym == sym_case_statement && child.named_child_count() > 1) {
      case_count++;
      if (child.child(0).text() == "default") {
        has_default = true;
      }
    }
  }

  for (const auto& child : body) {
    // skip cases without bodies
    if (child.sym == sym_case_statement && child.named_child_count() > 1) {
      call->_module_inst->visit([=](MtInstance* m) { m->push_state(child); });
      err << trace_case_statement(call, child);
      call->_module_inst->visit([=](MtInstance* m) { m->swap_state(MnNode::null); });
    }
  }

  if (has_default) {
    call->_module_inst->visit([=](MtInstance* m) { m->pop_state(MnNode::null); });
    case_count--;
  }

  for (int i = 0; i < case_count; i++) {
    call->_module_inst->visit([=](MtInstance* m) { m->merge_state(MnNode::null); });
  }
#endif
  return err;
}

//------------------------------------------------------------------------------

#endif
