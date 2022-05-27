#include "MtTracer.h"

#include "Log.h"
#include "MtChecker.h"
#include "MtContext.h"
#include "MtMethod.h"
#include "MtModLibrary.h"
#include "MtModule.h"
#include "MtNode.h"
#include "MtUtils.h"
#include "metron_tools.h"

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_method(MtContext* mod_ctx, MtMethod* method) {
  Err err;
  auto method_ctx = mod_ctx->resolve(method->name());

  auto node_body = method->_node.get_field(field_body);
  if (MtChecker::has_non_terminal_return(node_body)) {
    err << ERR("Method %s has non-terminal return\n", method->cname());
  }

  err << trace_sym_function_definition(method_ctx, method->_node);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::log_action(MtContext* method_ctx, MtContext* dst_ctx,
                                      ContextAction action,
                                      SourceRange source) {
  Err err;
  assert(method_ctx->context_type == CTX_METHOD);
  assert(method_ctx);
  assert(dst_ctx);

  if (action == CTX_WRITE) {
    if (dst_ctx->context_type != CTX_RETURN) {
      method_ctx->method->writes.insert(dst_ctx);
    }
  }

  auto old_state = dst_ctx->log_top.state;
  auto new_state = merge_action(old_state, action);
  dst_ctx->log_top.state = new_state;

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_identifier(MtContext* ctx, MnNode node,
                                            ContextAction action) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);

  switch (node.sym) {
    case sym_qualified_identifier:
    case alias_sym_namespace_identifier:
      // pretty sure these can't do anything
      break;
    case sym_identifier:
    case alias_sym_field_identifier: {
      assert(ctx->method);
      auto field_ctx = ctx->resolve(node.text());
      if (field_ctx) {
        err << log_action(ctx, field_ctx, action, node.get_source());
      }
      break;
    }
    default:
      err << trace_default(ctx, node, action);
      break;
  }
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_expression(MtContext* ctx, MnNode node,
                                            ContextAction action) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);

  switch (node.sym) {
    case sym_identifier:
    case sym_qualified_identifier:
      err << trace_identifier(ctx, node, action);
      break;
    case sym_conditional_expression:
      err << trace_sym_conditional_expression(ctx, node);
      break;
    case sym_field_expression:
      err << trace_sym_field_expression(ctx, node, action);
      break;
    case sym_subscript_expression:
      err << trace_expression(ctx, node.get_field(field_index), CTX_READ);
      err << trace_expression(ctx, node.get_field(field_argument), action);
      break;
    case sym_call_expression:
      err << trace_sym_call_expression(ctx, node);
      break;
    case sym_update_expression:
      // this is "i++" or similar, which is a read and a write.
      err << trace_expression(ctx, node.get_field(field_argument), CTX_READ);
      err << trace_expression(ctx, node.get_field(field_argument), CTX_WRITE);
      break;
    case sym_assignment_expression:
      err << trace_sym_assignment_expression(ctx, node);
      break;
    case sym_parenthesized_expression:
      err << trace_expression(ctx, node.child(1), action);
      break;

    case sym_unary_expression:
      err << trace_expression(ctx, node.get_field(field_argument), CTX_READ);
      break;

    case sym_binary_expression:
      err << trace_sym_binary_expression(ctx, node);
      break;

      /*
      case sym_condition_clause:
        err << trace_expression(ctx, node.child(1), action);
        break;
      */

    default:
      err << trace_default(ctx, node, action);
      break;
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_statement(MtContext* ctx, MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);

  switch (node.sym) {
    case sym_compound_statement:
      err << trace_sym_compound_statement(ctx, node);
      break;
    case sym_case_statement:
      err << trace_sym_case_statement(ctx, node);
      break;
    case sym_break_statement:
      err << trace_sym_break_statement(ctx, node);
      break;
    case sym_if_statement:
      err << trace_sym_if_statement(ctx, node);
      break;
    case sym_expression_statement:
      err << trace_expression(ctx, node.child(0), CTX_READ);
      break;
    case sym_switch_statement:
      err << trace_sym_switch_statement(ctx, node);
      break;
    case sym_return_statement:
      err << trace_sym_return_statement(ctx, node);
      break;
    case sym_for_statement:
      err << trace_sym_for_statement(ctx, node);
      break;

    default:
      err << trace_default(ctx, node);
      break;
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_declarator(MtContext* ctx, MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);

  switch (node.sym) {
    case sym_identifier:
      err << trace_identifier(ctx, node, CTX_READ);
      break;
    case sym_init_declarator:
      err << trace_sym_init_declarator(ctx, node);
      break;
    default:
      err << trace_default(ctx, node);
      break;
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_assignment_expression(MtContext* ctx, MnNode node) {
  Err err;

  auto op = node.get_field(field_operator).text();

  if (op == "=") {
    err << trace_expression(ctx, node.get_field(field_right), CTX_READ);
    err << trace_expression(ctx, node.get_field(field_left), CTX_WRITE);
  }
  else {
    err << trace_expression(ctx, node.get_field(field_right), CTX_READ);
    err << trace_expression(ctx, node.get_field(field_left), CTX_READ);
    err << trace_expression(ctx, node.get_field(field_left), CTX_WRITE);
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_for_statement(MtContext* ctx, MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);

  for (auto c : node) {
    if (c.field == field_initializer) {
      if (c.sym == sym_declaration) {
        err << trace_sym_declaration(ctx, c);
      }
      else {
        err << trace_expression(ctx, c, CTX_READ);
      }
    }
    else if (c.is_expression()) {
      err << trace_expression(ctx, c, CTX_READ);
    }
    else if (c.is_statement()) {
      err << trace_statement(ctx, c);
    }
    else {
      err << trace_default(ctx, c);
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_if_statement(MtContext* ctx, MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  assert(node.sym == sym_if_statement);

  auto node_cond = node.get_field(field_condition);
  auto node_branch_a = node.get_field(field_consequence);
  auto node_branch_b = node.get_field(field_alternative);

  err << trace_sym_condition_clause(ctx, node_cond);

  ctx_root->start_branch_a();
  if (!node_branch_a.is_null()) {
    err << trace_statement(ctx, node_branch_a);
  }
  ctx_root->end_branch_a();

  ctx_root->start_branch_b();
  if (!node_branch_b.is_null()) {
    err << trace_statement(ctx, node_branch_b);
  }
  ctx_root->end_branch_b();

  return err;
}

//------------------------------------------------------------------------------

// TREESITTER BUG - it isn't parsing "new_pad_x = pad_x + quad_dir ? 1 : 0;"
// correctly

/*
[000.009] ========== tree dump begin
[000.009]  � conditional_expression =
[000.009]  ���� condition: assignment_expression =
[000.009]  �  ���� left: identifier = "new_pad_x"
[000.009]  �  ���� operator: lit = "="
[000.009]  �  ���� right: binary_expression =
[000.009]  �     ���� left: identifier = "pad_x"
[000.009]  �     ���� operator: lit = "+"
[000.009]  �     ���� right: identifier = "quad_dir"
[000.009]  ���� lit = "?"
[000.009]  ���� consequence: number_literal = "1"
[000.009]  ���� lit = ":"
[000.009]  ���� alternative: number_literal = "0"
*/

CHECK_RETURN Err MtTracer::trace_sym_conditional_expression(MtContext* ctx,
                                                            MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  assert(node.sym == sym_conditional_expression);

  auto node_cond = node.get_field(field_condition);
  auto node_branch_a = node.get_field(field_consequence);
  auto node_branch_b = node.get_field(field_alternative);

  err << trace_expression(ctx, node_cond, CTX_READ);

  ctx_root->start_branch_a();
  if (!node_branch_a.is_null()) {
    err << trace_expression(ctx, node_branch_a, CTX_READ);
  }
  ctx_root->end_branch_a();

  ctx_root->start_branch_b();
  if (!node_branch_b.is_null()) {
    err << trace_expression(ctx, node_branch_b, CTX_READ);
  }
  ctx_root->end_branch_b();

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_call(MtContext* src_ctx, MtContext* dst_ctx,
                                      MnNode node_call) {
  Err err;

  if (!dst_ctx) return err;
  assert(src_ctx->context_type == CTX_METHOD);
  assert(dst_ctx->context_type == CTX_METHOD);

  // If the source and dest functions are not in the same module and the source
  // module has to pass params to the dest module, we have to bind the params
  // to ports to "call" it.

  bool cross_mod_call = src_ctx->method->_mod != dst_ctx->method->_mod;

  if (cross_mod_call && dst_ctx->method->has_params()) {
    err << log_action(src_ctx, dst_ctx, CTX_WRITE, node_call.get_source());
  }

  err << trace_sym_function_definition(dst_ctx, dst_ctx->method->_node);

  if (cross_mod_call && dst_ctx->method->has_params()) {
    err << log_action(src_ctx, dst_ctx, CTX_READ, node_call.get_source());
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_break_statement(MtContext* ctx,
                                                     MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  assert(node.sym == sym_break_statement);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_compound_statement(MtContext* ctx,
                                                        MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  assert(node.sym == sym_compound_statement);

  for (const auto& child : node) {
    if (child.sym == sym_declaration) {
      err << trace_sym_declaration(ctx, child);
    } else if (child.is_statement()) {
      err << trace_statement(ctx, child);
    } else {
      err << trace_default(ctx, child);
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_declaration(MtContext* ctx, MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  assert(node.sym == sym_declaration);

  auto node_type = node.get_field(field_type);
  auto node_decl = node.get_field(field_declarator);

  err << trace_declarator(ctx, node_decl);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_case_statement(MtContext* ctx,
                                                    MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  assert(node.sym == sym_case_statement);

  // Everything after the colon should be statements.

  bool hit_colon = false;
  for (auto child : node) {
    if (child.sym == anon_sym_COLON) {
      hit_colon = true;
      continue;
    }
    if (hit_colon) {
      err << trace_statement(ctx, child);
    }
  }

  if (!MtChecker::ends_with_break(node)) {
    err << ERR("Case statement in %s does not end with break\n",
               ctx->method->cname());
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_function_definition(MtContext* ctx,
                                                         MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  assert(node.sym == sym_function_definition);

  auto node_type = node.get_field(field_type);
  auto node_decl = node.get_field(field_declarator);
  auto node_body = node.get_field(field_body);

  err << trace_sym_compound_statement(ctx, node_body);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_call_expression(MtContext* ctx,
                                                     MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  assert(node.sym == sym_call_expression);

  if (!ctx->method) return err << ERR("Context does not contain a method\n");

  auto node_func = node.get_field(field_function);
  auto node_args = node.get_field(field_arguments);

  // Trace the args first.
  err << trace_sym_argument_list(ctx, node_args);

  // Now trace the actual call.
  switch (node_func.sym) {
    case sym_field_expression: {
      auto child_name = node_func.get_field(field_argument).text();
      auto child_ctx = ctx->resolve(child_name);
      if (!child_ctx) return err << ERR("Child context missing\n");

      auto child_func = node_func.get_field(field_field).text();
      err << trace_call(ctx, child_ctx->resolve(child_func), node);
      break;
    }
    case sym_identifier:
      err << trace_call(ctx, ctx->resolve(node_func.text()), node);
      break;

    case sym_template_function: {
      // FIXME this is a stub, we don't currently have real template function
      // support
      auto node_name =
          node.get_field(field_function).get_field(field_name).text();
      if (node_name == "bx" || node_name == "dup" ||
          node_name == "sign_extend") {
      } else {
        err << ERR("trace_sym_call_expression - Unhandled template func %s\n", node.text().c_str());
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

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_switch_statement(MtContext* ctx,
                                                      MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  assert(node.sym == sym_switch_statement);

  err << trace_sym_condition_clause(ctx, node.get_field(field_condition));

  auto body = node.get_field(field_body);

  bool has_default = false;
  for (const auto& child : body) {
    if (child.sym == sym_case_statement) {
      if (child.named_child_count() == 1) {
        continue;
      }
      if (child.child(0).text() == "default") {
        has_default = true;
      }
    }
  }

  ctx_root->start_switch();

  for (const auto& child : body) {
    if (child.sym == sym_case_statement) {
      // skip cases without bodies
      if (child.named_child_count() > 1) {
        ctx_root->start_case();
        err << trace_sym_case_statement(ctx, child);
        ctx_root->end_case();
      }
    }
  }

  if (!has_default) {
    ctx_root->start_case();
    ctx_root->end_case();
  }

  ctx_root->end_switch();

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_template_type(MtContext* ctx,
                                                   MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  assert(node.sym == sym_template_type);

  for (const auto& child : node) {
    switch (child.sym) {
      case sym_template_argument_list:
        err << trace_sym_template_argument_list(ctx, child);
        break;
      case alias_sym_type_identifier:
        err << trace_sym_type_identifier(ctx, child);
        break;
      default:
        err << trace_default(ctx, node);
        break;
    }
  }
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_type_identifier(MtContext* ctx,
                                                     MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  assert(node.sym == alias_sym_type_identifier);

  for (const auto& child : node) {
    switch (child.sym) {
      default:
        err << trace_default(ctx, node);
        break;
    }
  }
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_template_argument_list(MtContext* ctx,
                                                            MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  assert(node.sym == sym_template_argument_list);

  for (const auto& child : node) {
    switch (child.sym) {
      default:
        err << trace_default(ctx, node);
        break;
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_init_declarator(MtContext* ctx,
                                                     MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  assert(node.sym == sym_init_declarator);

  err << trace_expression(ctx, node.get_field(field_value), CTX_READ);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_return_statement(MtContext* ctx,
                                                      MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  assert(node.sym == sym_return_statement);

  auto node_lit = node.child(0);
  auto node_value = node.child(1);
  auto node_semi = node.child(2);

  err << trace_expression(ctx, node_value, CTX_READ);

  auto return_ctx = ctx->resolve("<return>");
  err << log_action(ctx, return_ctx, CTX_WRITE, node.get_source());

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_binary_expression(MtContext* ctx,
                                                       MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  assert(node.sym == sym_binary_expression);

  auto node_lhs = node.get_field(field_left);
  auto node_rhs = node.get_field(field_right);

  err << trace_expression(ctx, node_lhs, CTX_READ);
  err << trace_expression(ctx, node_rhs, CTX_READ);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_argument_list(MtContext* ctx,
                                                   MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  assert(node.sym == sym_argument_list);

  for (const auto& child : node) {
    if (child.is_identifier()) {
      err << trace_identifier(ctx, child, CTX_READ);
    } else if (child.is_expression()) {
      err << trace_expression(ctx, child, CTX_READ);
    } else {
      err << trace_default(ctx, child);
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_condition_clause(MtContext* ctx,
                                                      MnNode node) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  assert(node.sym == sym_condition_clause);

  auto node_value = node.get_field(field_value);
  err << trace_expression(ctx, node_value, CTX_READ);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_field_expression(MtContext* ctx,
                                                      MnNode node,
                                                      ContextAction action) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  assert(node.sym == sym_field_expression);

  auto field_ctx = ctx->resolve(node);

  if (field_ctx) {
    err << log_action(ctx, field_ctx, action, node.get_source());
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_default(MtContext* ctx, MnNode node,
                                         ContextAction action) {
  Err err;
  assert(ctx->context_type == CTX_METHOD);
  if (!node.is_named()) return err;

  switch (node.sym) {
    case sym_comment:
    case sym_using_declaration:
    case sym_number_literal:
    case sym_string_literal:
      break;
    default:
      // KCOV_OFF
      err << ERR("%s : No handler for %s\n", __func__, node.ts_node_type());
      node.error();
      break;
      // KCOV_ON
  }

  return err;
}
