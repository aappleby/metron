#include "CNodeStatement.hpp"

#include "CNodeExpression.hpp"

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

Err CNodeExpStatement::trace(IContext* context, TraceAction action) {
  return trace_children(context, action);
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
    err << child("rhs")->trace(context, ACT_READ);
    err << child("lhs")->trace(context, ACT_WRITE);
  }
  else {
    err << child("rhs")->trace(context, ACT_READ);
    err << child("lhs")->trace(context, ACT_READ);
    err << child("lhs")->trace(context, ACT_WRITE);
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
  if (auto value = child("value")) {
    err << value->trace(context, action);
  }
  return err;
}

//------------------------------------------------------------------------------
