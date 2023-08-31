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
  for (auto c : this) err << c->trace_read(context);
  return err;
}

//------------------------------------------------------------------------------

Err CNodeAssignment::emit(Cursor& c) {
  dump_tree();
  assert(false);
  return Err();
}

Err CNodeAssignment::trace(IContext* context, TraceAction action) {
  Err err;

  if (child("op")->get_text() == "=") {
    err << child("rhs")->trace_read(context);
    err << child("lhs")->trace_write(context);
  }
  else {
    err << child("rhs")->trace_read(context);
    err << child("lhs")->trace_read(context);
    err << child("lhs")->trace_write(context);
  }

  return Err();
}

//------------------------------------------------------------------------------

Err CNodeCompound::trace(IContext* context, TraceAction action) {
  Err err;
  for (auto c : this) err << c->trace(context, action);
  return err;
}

//------------------------------------------------------------------------------

Err CNodeReturn::trace(IContext* context, TraceAction action) {
  Err err;

  auto inst_return = dynamic_cast<CInstReturn*>(context->resolve("return"));
  err << inst_return->log_action(this, ACT_WRITE);

  return err;
}

//------------------------------------------------------------------------------
