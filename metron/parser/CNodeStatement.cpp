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

  auto lhs_name = lhs->get_name();
  auto inst_lhs = context->resolve(lhs_name);
  assert(inst_lhs);

  if (child("op")->get_text() == "=") {
    err << rhs->trace(context);
  }
  else {
    err << rhs->trace(context);
    err << lhs->trace(context);
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

  auto inst_return = dynamic_cast<CInstReturn*>(context->resolve("return"));
  err << inst_return->log_action(this, ACT_WRITE);

  return err;
}

//------------------------------------------------------------------------------
