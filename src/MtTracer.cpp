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
      err << log_action(ctx, field_ctx, action, n.get_source());
      break;
    }

    case sym_field_expression: {
      assert(false);
      /*
      auto node_arg = n.get_field(field_argument);
      auto field_ctx = ctx->resolve(node_arg.text());
      err << trace_dispatch(field_ctx, n.get_field(field_field), action);
      */
      break;
    }

    case sym_subscript_expression:
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
      for (const auto& c : n) err << trace_dispatch(ctx, c);
      break;

    case sym_comment:
    case sym_access_specifier:
    case sym_number_literal:
    case sym_primitive_type:
      break;

    default:
      printf("Don't know what to do with this:\n");
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

  MtContext* branch_a = ctx->clone();
  MtContext* branch_b = ctx->clone();

  if (!node_branch_a.is_null()) {
    err << trace_dispatch(branch_a, node_branch_a);
  }

  if (!node_branch_b.is_null()) {
    err << trace_dispatch(branch_b, node_branch_b);
  }

  err << merge_branch(branch_a, branch_b, ctx);

  delete branch_a;
  delete branch_b;

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_switch(MtContext* ctx, MnNode n) {
  Err err;

  err << trace_dispatch(ctx, n.get_field(field_condition));

  MtContext* old_ctx = ctx->clone();

  bool first_branch = true;

  auto body = n.get_field(field_body);
  for (const auto& c : body) {
    if (c.sym == sym_case_statement) {
      // case statement without body
      if (c.named_child_count() == 1) continue;

      if (first_branch) {
        err << trace_dispatch(ctx, c);
        first_branch = false;
      } else {
        MtContext* inst_case = old_ctx->clone();
        err << trace_dispatch(inst_case, c);
        err << merge_branch(ctx, inst_case, ctx);
        delete inst_case;
      }
    }
  }

  delete old_ctx;

  return err;
}

//-----------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::log_action(MtContext* method_ctx, MtContext* dst_ctx,
                                      ContextAction action,
                                      SourceRange source) {
  Err err;

  if (dst_ctx) {
    trace_log.push_back({method_ctx, dst_ctx, action, source});
    auto old_state = dst_ctx->state;
    auto new_state = merge_action(old_state, action);
    dst_ctx->state = new_state;

    if (dst_ctx->field && action == CTX_WRITE) {
      assert(method_ctx);
      dst_ctx->field->written_by = method_ctx->method;
    }
  }

  return err;
}

//-----------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::merge_branch(MtContext* ma, MtContext* mb,
                                        MtContext* out) {
  Err err;

  assert(ma->children.size() == out->children.size());
  assert(mb->children.size() == out->children.size());

  out->state = ::merge_branch(ma->state, mb->state);

  for (int i = 0; i < ma->children.size(); i++) {
    err << merge_branch(ma->children[i], mb->children[i], out->children[i]);
  }

  return err;
}

//------------------------------------------------------------------------------

void MtTracer::dump_log(MtField* filter_field) {
  for (const auto& l : trace_log) {
    if (filter_field) {
      if (l.dst_ctx->field != filter_field) continue;
    }

    auto ctx_path = l.dst_ctx->parent->name + "." + l.dst_ctx->name;
    LOG_G("%-10s %-10s %-20s @ ", to_string(l.action),
          to_string(l.dst_ctx->type), ctx_path.c_str());
    TinyLog::get().print_buffer(0, l.range.start,
                                l.range.end - l.range.start + 1);
    LOG("\n");
  }
}

//------------------------------------------------------------------------------
