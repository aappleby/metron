#include "MtTracer.h"

#include "Log.h"
#include "MtContext.h"
#include "MtMethod.h"
#include "MtModLibrary.h"
#include "MtModule.h"
#include "MtNode.h"
#include "MtUtils.h"
#include "metron_tools.h"

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_call(MtContext* method_ctx, MnNode node_call) {
  Err err;

  if (!method_ctx) return err;
  assert(method_ctx->type == CTX_METHOD);

  for (auto child_ctx : method_ctx->children) {
    if (child_ctx->type == CTX_PARAM) {
      err << log_action(method_ctx, child_ctx, CTX_WRITE,
                        node_call.get_source());
    }
  }

  err << trace_sym_function_definition(method_ctx, method_ctx->method->_node);

  for (auto child_ctx : method_ctx->children) {
    if (child_ctx->type == CTX_RETURN) {
      err << log_action(method_ctx, child_ctx, CTX_READ,
                        node_call.get_source());
    }
  }

  return err;
}

//-----------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::log_action(MtContext* method_ctx, MtContext* dst_ctx,
                                      ContextAction action,
                                      SourceRange source) {
  Err err;
  assert(method_ctx);
  assert(dst_ctx);

  if (action == CTX_WRITE) {
    if (dst_ctx->field) {
      method_ctx->method->writes.insert(dst_ctx->field);
    }
  }

  auto old_state = dst_ctx->log_top.state;
  auto new_state = merge_action(old_state, action);
  dst_ctx->log_top.state = new_state;

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_dispatch(MtContext* ctx, MnNode node,
                                          ContextAction action) {
  Err err;

  if (!ctx) return err;
  if (!node.is_named()) return err;

  // clang-format off
  switch (node.sym) {
    case sym_identifier:
    case alias_sym_field_identifier:
      err << trace_identifier(ctx, node, action);
      break;

    case sym_field_expression:
    case sym_subscript_expression:
    case sym_call_expression:
    case sym_assignment_expression:
    case sym_update_expression:
      err << trace_expression(ctx, node, action);
      break;

    case sym_return_statement:
    case sym_conditional_expression:
    case sym_if_statement:
    case sym_expression_statement:
      err << trace_statement(ctx, node, action);
      break;

    case sym_init_declarator:
    case sym_declaration:
    case sym_field_declaration:
    case sym_field_declaration_list:
    case alias_sym_type_identifier:
    case sym_class_specifier:
    case sym_binary_expression:
    case sym_parameter_declaration:
    case sym_argument_list:
    case sym_parameter_list:
    case sym_function_declarator:
    case sym_unary_expression:
    case sym_parenthesized_expression:
    case sym_condition_clause:
    case sym_template_type:
    case sym_comment:
    case sym_access_specifier:
    case sym_number_literal:
    case sym_template_argument_list:
    case sym_primitive_type:
      for (const auto& child : node) err << trace_dispatch(ctx, child, action);
      break;

    case sym_qualified_identifier:
    case alias_sym_namespace_identifier:
      err << trace_identifier(ctx, node, action);
      break;

    case sym_using_declaration:
      for (const auto& child : node) err << trace_dispatch(ctx, child, action);
      break;

    case sym_switch_statement:
    case sym_compound_statement:
    case sym_case_statement:
    case sym_break_statement:
      err << trace_statement(ctx, node, action); break;

    default:
      err << ERR("%s : No handler for %s\n", __func__, node.ts_node_type());
      node.error();
      break;
  }
  // clang-format on

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_identifier(MtContext* ctx, MnNode node,
                                            ContextAction action) {
  Err err;
  switch (node.sym) {
    case sym_qualified_identifier:
      for (const auto& child : node) err << trace_dispatch(ctx, child, action);
      break;
    case alias_sym_namespace_identifier:
      for (const auto& child : node) err << trace_dispatch(ctx, child, action);
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
      err << ERR("%s : No handler for %s\n", __func__, node.ts_node_type());
      node.error();
      break;
  }
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_expression(MtContext* ctx, MnNode node,
                                            ContextAction action) {
  Err err;

  switch (node.sym) {
    case sym_number_literal:
      break;
    case sym_identifier:
    case sym_qualified_identifier:
      err << trace_identifier(ctx, node, action);
      break;

    case sym_field_expression: {
      err << trace_sym_field_expression(ctx, node, action);
      break;
    }
    case sym_subscript_expression:
      err << trace_dispatch(ctx, node.get_field(field_index), CTX_READ);
      err << trace_dispatch(ctx, node.get_field(field_argument), action);
      break;
    case sym_call_expression:
      err << trace_sym_call_expression(ctx, node);
      break;
    case sym_update_expression:
      // this is "i++" or similar, which is a read and a write.
      err << trace_dispatch(ctx, node.get_field(field_argument), CTX_READ);
      err << trace_dispatch(ctx, node.get_field(field_argument), CTX_WRITE);
      break;
    case sym_assignment_expression:
      err << trace_dispatch(ctx, node.get_field(field_right), CTX_READ);
      err << trace_dispatch(ctx, node.get_field(field_left), CTX_WRITE);
      break;
    case sym_binary_expression:
      err << trace_sym_binary_expression(ctx, node);
      break;

    default:
      err << ERR("%s : No handler for %s\n", __func__, node.ts_node_type());
      node.error();
      break;
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_statement(MtContext* ctx, MnNode node,
                                           ContextAction action) {
  Err err;

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
    case sym_conditional_expression:
      err << trace_sym_if_statement(ctx, node);
      break;
    case sym_if_statement:
      err << trace_sym_if_statement(ctx, node);
      break;
    case sym_expression_statement:
      for (const auto& child : node) {
        err << trace_dispatch(ctx, child, action);
      }
      break;
    case sym_switch_statement:
      err << trace_sym_switch_statement(ctx, node);
      break;
    case sym_return_statement: {
      err << trace_sym_return_statement(ctx, node);
      break;
    }

    default:
      err << ERR("%s : No handler for %s\n", __func__, node.ts_node_type());
      node.error();
      break;
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_declarator(MtContext* ctx, MnNode node) {
  Err err;

  switch (node.sym) {
    case sym_identifier:
      err << trace_identifier(ctx, node, CTX_READ);
      break;

    case sym_init_declarator:
      err << trace_sym_init_declarator(ctx, node);
      break;

    default:
      err << ERR("%s : No handler for %s\n", __func__, node.ts_node_type());
      node.error();
      break;
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_break_statement(MtContext* ctx,
                                                     MnNode node) {
  Err err;
  assert(node.sym == sym_break_statement);

  for (const auto& child : node) {
    err << trace_dispatch(ctx, child);
  }
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_compound_statement(MtContext* ctx,
                                                        MnNode node) {
  Err err;
  assert(node.sym == sym_compound_statement);

  for (const auto& child : node) {
    switch (child.sym) {
      case anon_sym_LBRACE:
      case anon_sym_RBRACE:
      case sym_comment:
      case sym_using_declaration:
        break;

      case sym_declaration:
        err << trace_sym_declaration(ctx, child);
        break;

      case sym_return_statement:
      case sym_expression_statement:
      case sym_switch_statement:
      case sym_break_statement:
      case sym_if_statement:
        err << trace_statement(ctx, child, CTX_READ);
        break;

      case sym_compound_statement:
        err << trace_sym_compound_statement(ctx, child);
        break;

      default:
        err << ERR("%s : No handler for %s\n", __func__, child.ts_node_type());
        child.error();
        break;
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_declaration(MtContext* ctx, MnNode node) {
  Err err;
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
  assert(node.sym == sym_case_statement);

  for (const auto& child : node) {
    err << trace_dispatch(ctx, child);
  }
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_function_definition(MtContext* ctx,
                                                         MnNode node) {
  Err err;
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
  assert(node.sym == sym_call_expression);

  auto node_func = node.get_field(field_function);
  auto node_args = node.get_field(field_arguments);

  // Trace the args first.
  err << trace_dispatch(ctx, node_args);

  // Now trace the actual call.
  switch (node_func.sym) {
    case sym_field_expression: {
      auto child_name = node_func.get_field(field_argument).text();
      auto child_ctx = ctx->resolve(child_name);
      assert(child_ctx);

      auto child_func = node_func.get_field(field_field).text();
      err << trace_call(child_ctx->resolve(child_func), node);
      break;
    }
    case sym_identifier:
      err << trace_call(ctx->resolve(node_func.text()), node);
      break;

    case sym_template_function: {
      // FIXME this is a stub, we don't currently have real template function
      // support
      auto node_name =
          node.get_field(field_function).get_field(field_name).text();
      if (node_name == "bx" || node_name == "dup" ||
          node_name == "sign_extend") {
      } else {
        debugbreak();
      }
      break;
    }

    default:
      err << ERR("%s : No handler for %s\n", __func__, node.ts_node_type());
      node.error();
      break;
  }

  return err;
}

//------------------------------------------------------------------------------
// good

CHECK_RETURN Err MtTracer::trace_sym_if_statement(MtContext* ctx, MnNode node) {
  Err err;
  assert(node.sym == sym_if_statement ||
         node.sym == sym_conditional_expression);

  auto node_cond = node.get_field(field_condition);
  auto node_branch_a = node.get_field(field_consequence);
  auto node_branch_b = node.get_field(field_alternative);

  err << trace_dispatch(ctx, node_cond);

  ctx_root->start_branch_a();
  if (!node_branch_a.is_null()) {
    err << trace_dispatch(ctx, node_branch_a);
  }
  ctx_root->end_branch_a();

  ctx_root->start_branch_b();
  if (!node_branch_b.is_null()) {
    err << trace_dispatch(ctx, node_branch_b);
  }
  ctx_root->end_branch_b();

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_switch_statement(MtContext* ctx,
                                                      MnNode node) {
  Err err;
  assert(node.sym == sym_switch_statement);

  err << trace_dispatch(ctx, node.get_field(field_condition));

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
        err << ERR("%s : No handler for %s\n", __func__, child.ts_node_type());
        child.error();
        break;
    }
  }
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_type_identifier(MtContext* ctx,
                                                     MnNode node) {
  Err err;
  assert(node.sym == alias_sym_type_identifier);

  for (const auto& child : node) {
    switch (child.sym) {
      default:
        err << ERR("%s : No handler for %s\n", __func__, child.ts_node_type());
        child.error();
        break;
    }
  }
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_template_argument_list(MtContext* ctx,
                                                            MnNode node) {
  Err err;
  assert(node.sym == sym_template_argument_list);

  for (const auto& child : node) {
    switch (child.sym) {
      case anon_sym_LT:
      case anon_sym_GT:
      case sym_number_literal:
      case sym_type_descriptor:
        break;

      default:
        err << ERR("%s : No handler for %s\n", __func__, child.ts_node_type());
        node.error();
        break;
    }
  }
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_init_declarator(MtContext* ctx,
                                                     MnNode node) {
  Err err;
  assert(node.sym == sym_init_declarator);

  auto node_decl = node.child(0);
  auto node_eq = node.child(1);
  auto node_value = node.child(2);

  err << trace_expression(ctx, node_value, CTX_READ);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_return_statement(MtContext* ctx,
                                                      MnNode node) {
  Err err;
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
  assert(node.sym == sym_binary_expression);

  auto node_lhs = node.get_field(field_left);
  auto node_rhs = node.get_field(field_right);

  err << trace_expression(ctx, node_lhs, CTX_READ);
  err << trace_expression(ctx, node_rhs, CTX_READ);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_sym_field_expression(MtContext* ctx,
                                                      MnNode node,
                                                      ContextAction action) {
  Err err;
  assert(node.sym == sym_field_expression);

  auto node_arg = node.get_field(field_argument);
  auto node_field = node.get_field(field_field);

  auto component_ctx = ctx->resolve(node_arg.text());
  if (component_ctx) {
    auto field_ctx = component_ctx->resolve(node_field.text());
    err << log_action(ctx, field_ctx, action, node.get_source());
  } else {
    // Local struct field reference, don't need to trace it.
  }

  return err;
}

//------------------------------------------------------------------------------
