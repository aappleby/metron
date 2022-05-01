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

CHECK_RETURN Err MtTracer::trace_dispatch(MtContext* ctx, MnNode n,
                                          ContextAction action) {
  Err err;

  if (!ctx) return err;
  if (!n.is_named()) return err;

  switch (n.sym) {
    case sym_identifier:
    case alias_sym_field_identifier: {
      assert(ctx->method);

      auto field_ctx = ctx->resolve(n.text());
      if (field_ctx) {
        err << log_action(ctx, field_ctx, action, n.get_source());
      }
      break;
    }

    case sym_field_expression: {
      /*
      n.dump_source_lines();
      n.dump_tree();
      assert(false);
      */
      auto node_arg = n.get_field(field_argument);
      auto node_field = n.get_field(field_field);

      auto component_ctx = ctx->resolve(node_arg.text());
      auto field_ctx = component_ctx->resolve(node_field.text());

      err << log_action(ctx, field_ctx, action, n.get_source());
      // err << trace_dispatch(field_ctx, node_field, action);
      break;
    }

    case sym_subscript_expression:
      err << trace_dispatch(ctx, n.get_field(field_index), CTX_READ);
      err << trace_dispatch(ctx, n.get_field(field_argument), action);
      break;

    case sym_call_expression:
      err << trace_call(ctx, n);
      break;

    case sym_conditional_expression:
    case sym_if_statement:
      err << trace_branch(ctx, n);
      break;

    case sym_switch_statement:
      err << trace_switch(ctx, n);
      break;

    case sym_update_expression: {
      // this is "i++" or similar, which is a read and a write.
      err << trace_dispatch(ctx, n.get_field(field_argument), CTX_READ);
      err << trace_dispatch(ctx, n.get_field(field_argument), CTX_WRITE);
      break;
    }

    case sym_assignment_expression:
      err << trace_dispatch(ctx, n.get_field(field_right), CTX_READ);
      err << trace_dispatch(ctx, n.get_field(field_left), CTX_WRITE);
      break;

    case sym_function_definition: {
      err << trace_dispatch(ctx, n.get_field(field_body), action);
      break;
    }

    case sym_return_statement: {
      assert(ctx->method);
      for (const auto& c : n) err << trace_dispatch(ctx, c);

      auto return_ctx = ctx->resolve("<return>");
      err << log_action(ctx, return_ctx, CTX_WRITE, n.get_source());
      break;
    }

    case sym_init_declarator:
    case sym_declaration:
    case sym_field_declaration:
    case sym_field_declaration_list:
    case alias_sym_type_identifier:
    case sym_class_specifier:
    case sym_binary_expression:
    case sym_parameter_declaration:
    case sym_argument_list:
    case sym_expression_statement:
    case sym_compound_statement:
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
    case sym_qualified_identifier:
    case alias_sym_namespace_identifier:
    case sym_using_declaration:
    case sym_case_statement:
    case sym_break_statement:
      for (const auto& c : n) err << trace_dispatch(ctx, c);
      break;

    default:
      err << ERR("Don't know what to do with %s\n", n.ts_node_type());
      n.dump_tree();
      exit(0);
      break;
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_call(MtContext* ctx, MnNode n) {
  Err err;
  if (!ctx) return err;

  auto node_func = n.get_field(field_function);
  auto node_args = n.get_field(field_arguments);

  // Trace the args first.
  err << trace_dispatch(ctx, node_args);

  //----------

  if (node_func.sym == sym_field_expression) {
    auto child_name = node_func.get_field(field_argument).text();
    auto child_ctx = ctx->resolve(child_name);
    assert(child_ctx);

    auto child_func = node_func.get_field(field_field).text();
    err << trace_method_ctx(child_ctx->resolve(child_func), n);
  }

  //----------

  else if (node_func.sym == alias_sym_field_identifier) {
    LOG_R("When do we hit this?");
    debugbreak();

    err << trace_method_ctx(ctx->resolve(node_func.text()), n);
  }

  //----------

  else if (node_func.sym == sym_identifier) {
    err << trace_method_ctx(ctx->resolve(node_func.text()), n);
  }

  //----------

  else if (node_func.sym == sym_template_function) {
    // FIXME this is a stub, we don't currently have real template function
    // support
    auto node_name = n.get_field(field_function).get_field(field_name).text();
    if (node_name == "bx" || node_name == "dup" || node_name == "sign_extend") {
    } else {
      debugbreak();
    }
    return err;

  }

  //----------

  else {
    err << ERR("Don't know what to do with %s\n", node_func.ts_node_type());
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_method_ctx(MtContext* method_ctx,
                                            MnNode node_call) {
  Err err;

  if (!method_ctx) return err;
  assert(method_ctx->type == CTX_METHOD);

  for (auto child_ctx : method_ctx->children) {
    if (child_ctx->type == CTX_PARAM) {
      err << log_action(method_ctx, child_ctx, CTX_WRITE,
                        node_call.get_source());
    }
  }

  err << trace_dispatch(method_ctx, method_ctx->method->_node);

  for (auto child_ctx : method_ctx->children) {
    if (child_ctx->type == CTX_RETURN) {
      err << log_action(method_ctx, child_ctx, CTX_READ,
                        node_call.get_source());
    }
  }

  return err;
}

//------------------------------------------------------------------------------
// good

CHECK_RETURN Err MtTracer::trace_branch(MtContext* ctx, MnNode n) {
  Err err;

  auto node_cond = n.get_field(field_condition);
  auto node_branch_a = n.get_field(field_consequence);
  auto node_branch_b = n.get_field(field_alternative);

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

CHECK_RETURN Err MtTracer::trace_switch(MtContext* ctx, MnNode n) {
  Err err;

  err << trace_dispatch(ctx, n.get_field(field_condition));

  auto body = n.get_field(field_body);

  bool has_default = false;
  for (const auto& c : body) {
    if (c.sym == sym_case_statement) {
      if (c.named_child_count() == 1) {
        continue;
      }
      if (c.child(0).text() == "default") {
        has_default = true;
      }
    }
  }

  ctx_root->start_switch();

  for (const auto& c : body) {
    if (c.sym == sym_case_statement) {
      if (c.named_child_count() == 1) {
        continue;  // case statement without body
      }

      ctx_root->start_case();
      err << trace_dispatch(ctx, c);
      ctx_root->end_case();
    }
  }

  if (!has_default) {
    ctx_root->start_case();
    ctx_root->end_case();
  }

  ctx_root->end_switch();

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
