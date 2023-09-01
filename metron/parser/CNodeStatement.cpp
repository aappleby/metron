#include "CNodeStatement.hpp"

#include "CNodeExpression.hpp"

#include "CInstance.hpp"

//------------------------------------------------------------------------------

uint32_t CNodeStatement::debug_color() const {
  return COL_TEAL;
}

//------------------------------------------------------------------------------

Err CNodeExpStatement::emit(Cursor& c) {
  dump_tree();
  assert(false);
  return Err();
}

Err CNodeExpStatement::trace(IContext* context) {
  Err err;
  for (auto c : this) err << c->trace(context);
  return err;
}

//------------------------------------------------------------------------------

Err CNodeAssignment::emit(Cursor& c) {
  dump_tree();
  assert(false);
  return Err();
}

Err CNodeAssignment::trace(IContext* context) {
  Err err;

  auto rhs = child("rhs");
  auto lhs = child("lhs");

  auto inst_lhs = context->resolve(lhs);
  assert(inst_lhs);

  err << rhs->trace(context);

  if (child("op")->get_text() != "=") {
    err << inst_lhs->log_action(this, ACT_READ);
  }

  err << inst_lhs->log_action(this, ACT_WRITE);

  return Err();
}

//------------------------------------------------------------------------------

Err CNodeCompound::trace(IContext* context) {
  Err err;
  for (auto c : this) err << c->trace(context);
  return err;
}

//------------------------------------------------------------------------------

Err CNodeReturn::trace(IContext* context) {
  Err err;

  if (auto value = child("value")) {
    err << value->trace(context);
  }

  auto inst_return = context->resolve(this);
  if (inst_return) {
    err << inst_return->log_action(this, ACT_WRITE);
  }

  return err;
}

//------------------------------------------------------------------------------
