#include "MtTracer.h"

#include "Log.h"
#include "MtContext.h"
#include "MtMethod.h"
#include "MtModLibrary.h"
#include "MtModule.h"
#include "MtNode.h"
#include "metron_tools.h"

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_dispatch(MtContext* inst, MnNode n,
                                          FieldAction action) {
  Err err;

  if (!inst) return err;

  switch (n.sym) {
    case sym_identifier: {
      auto inst1 = inst->get_child(n.text());
      if (inst1) err << log_action(inst1, action, n.get_source());
      break;
    }

    case sym_field_expression: {
      auto node_arg = n.get_field(field_argument);
      auto node_field = n.get_field(field_field);

      auto inst1 = inst->get_child(node_arg.text());
      if (inst1) {
        auto inst2 = inst1->get_child(node_field.text());
        if (inst2) {
          err << log_action(inst2, action, n.get_source());
        }
      }
      break;
    }

    case sym_subscript_expression:
      err << trace_dispatch(inst, n.get_field(field_argument), action);
      break;

    case sym_call_expression:
      err << trace_call(inst, n);
      break;

    case sym_conditional_expression:
    case sym_if_statement:
      err << trace_branch(inst, n);
      break;

    case sym_switch_statement:
      err << trace_switch(inst, n);
      break;

    case sym_update_expression: {
      // this is "i++" or similar, which is a read and a write.
      err << trace_dispatch(inst, n.get_field(field_argument), FIELD_READ);
      err << trace_dispatch(inst, n.get_field(field_argument), FIELD_WRITE);
      break;
    }

    case sym_assignment_expression:
      err << trace_dispatch(inst, n.get_field(field_right), FIELD_READ);
      err << trace_dispatch(inst, n.get_field(field_left), FIELD_WRITE);
      break;

    default:
      for (const auto& c : n) err << trace_dispatch(inst, c);
      break;
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_call(MtContext* inst, MnNode n) {
  Err err;
  if (!inst) return err;

  auto node_func = n.get_field(field_function);
  auto node_args = n.get_field(field_arguments);

  // Trace the args first.
  err << trace_dispatch(inst, node_args);

  //----------

  if (node_func.sym == sym_field_expression) {
    err << trace_call(
        inst->get_child(node_func.get_field(field_argument).text()),
        node_func.get_field(field_field));
  }

  //----------

  else if (node_func.sym == sym_identifier) {
    auto dst_method = inst->mod->get_method(node_func.text());
    if (dst_method) {
      err << trace_dispatch(inst, dst_method->_node);
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
    err << ERR("MtModule::build_call_tree - don't know what to do with %s\n",
               node_func.ts_node_type());
  }

  return err;
}

//------------------------------------------------------------------------------
// good

CHECK_RETURN Err MtTracer::trace_branch(MtContext* inst, MnNode n) {
  Err err;

  auto node_cond = n.get_field(field_condition);
  auto node_branch_a = n.get_field(field_consequence);
  auto node_branch_b = n.get_field(field_alternative);

  err << trace_dispatch(inst, node_cond);

  MtContext branch_a = *inst;
  MtContext branch_b = *inst;

  if (!node_branch_a.is_null()) {
    err << trace_dispatch(&branch_a, node_branch_a);
  }

  if (!node_branch_b.is_null()) {
    err << trace_dispatch(&branch_b, node_branch_b);
  }

  err << merge_branch(&branch_a, &branch_b, inst);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_switch(MtContext* inst, MnNode n) {
  Err err;

  err << trace_dispatch(inst, n.get_field(field_condition));

  MtContext old_inst = *inst;

  bool first_branch = true;

  auto body = n.get_field(field_body);
  for (const auto& c : body) {
    if (c.sym == sym_case_statement) {
      // case statement without body
      if (c.named_child_count() == 1) continue;

      if (first_branch) {
        err << trace_dispatch(inst, c);
        first_branch = false;
      } else {
        MtContext inst_case = old_inst;
        err << trace_dispatch(&inst_case, c);
        err << merge_branch(inst, &inst_case, inst);
      }
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::log_action(MtContext* inst, FieldAction action,
                                      SourceRange source) {
  Err err;

  if (action == FIELD_WRITE) {
    switch (inst->state) {
      case FIELD_UNKNOWN:
        inst->state = FIELD_OUTPUT;
        break;
      case FIELD_INPUT:
        inst->state = FIELD_REGISTER;
        break;
      case FIELD_OUTPUT:
        inst->state = FIELD_OUTPUT;
        break;
      case FIELD_SIGNAL:
        inst->state = FIELD_INVALID;
        break;
      case FIELD_REGISTER:
        inst->state = FIELD_REGISTER;
        break;
      case FIELD_INVALID:
        inst->state = FIELD_INVALID;
        break;
      default:
        assert(false);
    }
  } else if (action == FIELD_READ) {
    switch (inst->state) {
      case FIELD_UNKNOWN:
        inst->state = FIELD_INPUT;
        break;
      case FIELD_INPUT:
        inst->state = FIELD_INPUT;
        break;
      case FIELD_OUTPUT:
        inst->state = FIELD_SIGNAL;
        break;
      case FIELD_SIGNAL:
        inst->state = FIELD_SIGNAL;
        break;
      case FIELD_REGISTER:
        inst->state = FIELD_INVALID;
        break;
      case FIELD_INVALID:
        inst->state = FIELD_INVALID;
        break;
      default:
        assert(false);
    }
  } else {
    err << ERR("Unknown field action %d\n", action);
  }

  return err;
}

//-----------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::merge_branch(MtContext* ma, MtContext* mb,
                                        MtContext* out) {
  Err err;

  assert(ma->children.size() == out->children.size());
  assert(mb->children.size() == out->children.size());

  if (ma->state == mb->state) {
    out->state = ma->state;
  } else if (ma->state == FIELD_INVALID || mb->state == FIELD_INVALID) {
    out->state = FIELD_INVALID;
  } else {
    if (ma->state == FIELD_UNKNOWN && mb->state == FIELD_INPUT)
      out->state = FIELD_INPUT;  // promote
    if (ma->state == FIELD_OUTPUT && mb->state == FIELD_SIGNAL)
      out->state = FIELD_SIGNAL;  // promote

    if (ma->state == FIELD_UNKNOWN && mb->state == FIELD_OUTPUT)
      out->state = FIELD_REGISTER;  // half-write, infer reg
    if (ma->state == FIELD_INPUT && mb->state == FIELD_OUTPUT)
      out->state = FIELD_REGISTER;  // half-write, infer reg
    if (ma->state == FIELD_UNKNOWN && mb->state == FIELD_REGISTER)
      out->state = FIELD_REGISTER;  // promote
    if (ma->state == FIELD_INPUT && mb->state == FIELD_REGISTER)
      out->state = FIELD_REGISTER;  // promote
    if (ma->state == FIELD_OUTPUT && mb->state == FIELD_REGISTER)
      out->state = FIELD_REGISTER;  // promote

    if (ma->state == FIELD_UNKNOWN && mb->state == FIELD_SIGNAL)
      out->state = FIELD_INVALID;  // half-write, bad signal
    if (ma->state == FIELD_INPUT && mb->state == FIELD_SIGNAL)
      out->state = FIELD_INVALID;  // order conflict

    if (ma->state == FIELD_SIGNAL && mb->state == FIELD_REGISTER)
      out->state = FIELD_INVALID;  // order conflict
  }

  for (int i = 0; i < ma->children.size(); i++) {
    err << merge_branch(ma->children[i], mb->children[i], out->children[i]);
  }

  return err;
}

//------------------------------------------------------------------------------
