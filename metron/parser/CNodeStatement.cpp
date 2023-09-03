#include "CNodeStatement.hpp"

#include "CNodeExpression.hpp"

#include "CInstance.hpp"

//------------------------------------------------------------------------------

uint32_t CNodeStatement::debug_color() const {
  return COL_TEAL;
}

//------------------------------------------------------------------------------

Err CNodeExpStatement::emit(Cursor& c) {
  NODE_ERR("Can't emit statement base class");
  return Err();
}

Err CNodeExpStatement::trace(CCall* call) {
  Err err;
  for (auto c : this) err << c->trace(call);
  return err;
}

//------------------------------------------------------------------------------

Err CNodeAssignment::emit(Cursor& c) {
  NODE_ERR("FIXME");
  return Err();
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

CHECK_RETURN Err CNodeIf::trace(CCall* call) {
  assert(false);
  return Err();
}

//------------------------------------------------------------------------------

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

  /*
  if (call->inst_return) {
    err << call->inst_return->log_action(this, ACT_WRITE);
  }
  */

  return err;
}

//------------------------------------------------------------------------------
