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

  assert(ctx->method);

  /*
  LOG("trace %s %s %s %s\n", to_string(ctx->type), ctx->name.c_str(),
      n.ts_node_type(), to_string(action));
  LOG_INDENT_SCOPE();
  */

  switch (n.sym) {
    case sym_identifier: {
      auto inst1 = ctx->resolve(n.text());
      if (inst1) err << log_action(inst1, action, n.get_source());
      break;
    }

    case sym_field_expression: {
      auto node_arg = n.get_field(field_argument);
      auto node_field = n.get_field(field_field);

      auto child_ctx = ctx->resolve(node_arg.text());
      if (child_ctx) {
        auto field_ctx = child_ctx->resolve(node_field.text());
        if (field_ctx) {
          err << log_action(field_ctx, action, n.get_source());
        }
      }
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
      // n.dump_tree();

      /*
      auto func_name =
          n.get_field(field_declarator).get_field(field_declarator).text();
      auto func_ctx = ctx->resolve(func_name);
      */

      /*
      if (func_ctx->method->callers.size()) {
        printf("%s some callers\n", func_ctx->method->cname());
      } else {
        printf("%s no callers\n", func_ctx->method->cname());
      }
      */
      // printf("%p\n", func_ctx);

      // auto dst_method =
      // n.dump_tree();

      for (const auto& c : n) err << trace_dispatch(ctx, c);
      break;
    }

    case sym_return_statement:
    case sym_field_declaration:
    case sym_field_declaration_list:
    case alias_sym_type_identifier:
    case sym_class_specifier:
    case sym_binary_expression:
    case sym_parameter_declaration:
    case sym_argument_list:
    case sym_expression_statement:
    case sym_compound_statement:
    case alias_sym_field_identifier:
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

CHECK_RETURN Err MtTracer::trace_method(MtContext ctx, MnNode n) {
  Err err;
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_call(MtContext* ctx, MnNode n) {
  Err err;
  if (!ctx) return err;

  LOG_G("Trace Call %s\n", n.text().c_str());

  auto node_func = n.get_field(field_function);
  auto node_args = n.get_field(field_arguments);

  // Trace the args first.
  err << trace_dispatch(ctx, node_args);

  //----------

  if (node_func.sym == sym_field_expression) {
    auto child_ctx = ctx->resolve(node_func.get_field(field_argument).text());
    auto child_func = node_func.get_field(field_field);

    auto dst_method = child_ctx->mod->get_method(child_func.text());
    if (dst_method) {
      auto method_ctx = child_ctx->get_child(dst_method->name());
      assert(method_ctx);
      if (dst_method->has_params) {
        err << log_action(method_ctx, CTX_WRITE, n.get_source());
      }
      if (dst_method->has_return) {
        err << log_action(method_ctx, CTX_READ, n.get_source());
      }

      // err << trace_dispatch(child_ctx, dst_method->_node);
      err << trace_dispatch(method_ctx, dst_method->_node);
    }
  }

  //----------

  else if (node_func.sym == alias_sym_field_identifier) {
    auto dst_method = ctx->mod->get_method(node_func.text());
    if (dst_method) {
      auto method_ctx = ctx->get_child(dst_method->name());
      assert(method_ctx);
      if (dst_method->has_params) {
        err << log_action(method_ctx, CTX_WRITE, n.get_source());
      }
      if (dst_method->has_return) {
        err << log_action(method_ctx, CTX_READ, n.get_source());
      }
      err << trace_dispatch(ctx, dst_method->_node);
    }
  }

  //----------

  else if (node_func.sym == sym_identifier) {
    auto dst_method = ctx->mod->get_method(node_func.text());
    if (dst_method) {
      err << trace_dispatch(ctx, dst_method->_node);
    }
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
    if (node_func.is_null()) {
      err << ERR("MtTracer::trace_call - node_func is null\n");

    } else {
      err << ERR("MtModule::build_call_tree - don't know what to do with %s\n",
                 node_func.ts_node_type());
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

CHECK_RETURN Err MtTracer::log_action(MtContext* ctx, ContextAction action,
                                      SourceRange source) {
  Err err;

  ctx->state = merge_action(ctx->state, action);

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
