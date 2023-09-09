#include "CNodeStatement.hpp"

#include "CNodeExpression.hpp"
#include "CNodeFunction.hpp"

#include "CInstance.hpp"

//------------------------------------------------------------------------------

uint32_t CNodeStatement::debug_color() const {
  return COL_TEAL;
}

//------------------------------------------------------------------------------

Err CNodeExpStatement::emit(Cursor& c) {
  return c.emit_default(this);
}

Err CNodeExpStatement::trace(CCall* call) {
  Err err;
  for (auto c : this) err << c->trace(call);
  return err;
}

//------------------------------------------------------------------------------

/*
//------------------------------------------------------------------------------
// Change '=' to '<=' if lhs is a field and we're inside a sequential block.
// Change "a += b" to "a = a + b", etc.

CHECK_RETURN Err MtCursor::emit_sym_assignment_expression(MnNode node) {
  Err err = check_at(sym_assignment_expression, node);

  MnNode lhs;

  for (auto child : node) {
    err << emit_ws_to(child);
    switch (child.field) {
      case field_left: {
        lhs = child;
        err << emit_dispatch(child);
        break;
      }
      case field_operator: {
        // There may not be a method if we're in an enum initializer list.
        bool left_is_field = current_mod.top()->get_field(lhs.name4()) != nullptr;
        if (current_method.top() && current_method.top()->is_tick_ && left_is_field) {
          err << emit_replacement(child, "<=");
        } else {
          err << emit_replacement(child, "=");
        }
        auto op  = child.text();
        bool is_compound = op != "=";
        if (is_compound) {
          push_cursor(lhs);
          err << emit_print(" ");
          err << emit_dispatch(lhs);
          err << emit_print(" %c", op[0]);
          pop_cursor();
        }
        break;
      }
      default: {
        err << emit_dispatch(child);
        break;
      }
    }
  }

  return err << check_done(node);
}
*/

Err CNodeAssignment::emit(Cursor& c) {
  Err err;

  //dump_tree();

  auto func = ancestor<CNodeFunction>();

  auto lhs = child("lhs");
  auto op  = child("op");
  auto rhs = child("rhs");

  err << c.emit(lhs);
  err << c.emit_gap_after(lhs);

  // If we're in a tick, emit < to turn = into <=
  if (func->method_type == MT_TICK) {
    err << c.emit_print("<");
  }

  if (op->get_text() == "=") {
    err << c.emit(op);
    err << c.emit_gap_after(op);
  }
  else {
    auto lhs_text = lhs->get_text();

    err << c.skip_over(op);
    err << c.emit_print("=");
    err << c.emit_gap_after(op);
    err << c.emit_print("%.*s %c ", lhs_text.size(), lhs_text.data(), op->get_text()[1]);
  }

  err << c.emit(rhs);
  err << c.emit_gap_after(rhs);

  return err;
}

Err CNodeAssignment::trace(CCall* call) {
  Err err;

  auto rhs = child("rhs");
  err << rhs->trace(call);

  auto lhs = child("lhs");
  auto inst_lhs = call->inst_class->resolve(lhs);

  if (inst_lhs) {
    auto op_text = child("op")->get_text();
    if (op_text != "=") err << inst_lhs->log_action(this, ACT_READ);
    err << inst_lhs->log_action(this, ACT_WRITE);
  }

  return Err();
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeFor::trace(CCall* call) {
  Err err;
  err << child("init")->trace(call);
  err << child("condition")->trace(call);
  err << child("body")->trace(call);
  err << child("step")->trace(call);
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeIf::trace(CCall* call) {
  Err err;

  err << child("condition")->trace(call);

  auto inst = call->inst_class;

  inst->push_state();
  if (auto body_true = child("body_true")) err << body_true->trace(call);
  inst->swap_state();
  if (auto body_false = child("body_false")) err << body_false->trace(call);
  inst->merge_state();

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeSwitch::trace(CCall* call) {
  Err err;

  err << child("condition")->trace(call);

  auto inst = call->inst_class;

  int case_count = 0;
  bool has_default = false;

  for (auto cursor = child("ldelim"); cursor; cursor = cursor->node_next) {
    if (cursor->tag_is("default")) has_default = true;

    // Skip cases without bodies
    if (!cursor->child("body")) continue;

    inst->push_state();
    case_count++;
    err << cursor->trace(call);
    inst->swap_state();
  }

  if (has_default) {
    inst->pop_state();
    case_count--;
  }

  for (int i = 0; i < case_count; i++) {
    inst->merge_state();
  }

  return err;
}

CHECK_RETURN Err CNodeCase::trace(CCall* call) {
  return child("body")->trace(call);
}

CHECK_RETURN Err CNodeDefault::trace(CCall* call) {
  return child("body")->trace(call);
}

//------------------------------------------------------------------------------

Err CNodeCompound::emit_block(Cursor& c, std::string ldelim, std::string rdelim) {
  Err err;

  for (auto child : this) {
    if (child->tag_is("ldelim")) {
      err << c.emit_replacement(child, "%s", ldelim.c_str());
    }
    else if (child->tag_is("rdelim")) {
      err << c.emit_replacement(child, "%s", rdelim.c_str());
    }
    else {
      err << c.emit(child);
    }
    err << c.emit_gap_after(child);
  }

  return err;
}

//----------------------------------------

Err CNodeCompound::trace(CCall* call) {
  Err err;
  for (auto c : this) err << c->trace(call);
  return err;
}

//------------------------------------------------------------------------------

Err CNodeReturn::trace(CCall* call) {
  Err err;

  if (auto node_value = child("value")) {
    err << node_value->trace(call);
  }

  return err;
}

Err CNodeReturn::emit(Cursor& cursor) {
  Err err;

  auto func = ancestor<CNodeFunction>();
  auto fname = func->get_namestr();

  auto node_ret = child("return");
  auto node_val = child("value");

  assert(node_val);

  err << cursor.skip_over(node_ret);
  err << cursor.skip_gap_after(node_ret);

  err << cursor.emit_print("%s_ret = ", fname.c_str());

  err << cursor.emit(node_val);
  err << cursor.emit_gap_after(node_val);

  return err;
}

//------------------------------------------------------------------------------
