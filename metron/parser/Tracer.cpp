#include "Tracer.hpp"

#include "CSourceRepo.hpp"
#include "CInstance.hpp"

#include "NodeTypes.hpp"

#if 0

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

CHECK_RETURN Err Tracer::trace_call(CInstCall* call, CNode* call_node) {
  Err err;

#if 0
  if (!method) {
    // This is a call to b32() or something similar. We should eventually check
    // that this is a known helper function, but for now we can ignore it.
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

CHECK_RETURN Err Tracer::trace_assignment_expression(CInstCall* call, CNode* node) {
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
  err << trace_expression(call, node->child("lhs")->as<CNodeExpression>(), ACT_READ);
  err << trace_expression(call, node->child("rhs")->as<CNodeExpression>(), ACT_READ);
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
      auto child_func_inst = child_inst->as<MtModuleInstance>()->get_method(child_func);
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
