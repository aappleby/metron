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

Err CNodeExpStatement::trace(CInstance* instance, TraceAction action) {
  return trace_children(instance, action);
}

//------------------------------------------------------------------------------

Err CNodeAssignment::emit(Cursor& c) {
  dump_tree();
  assert(false);
  return Err();
}

Err CNodeAssignment::trace(CInstance* instance, TraceAction action) {
  Err err;

  if (child("op")->get_text() == "=") {
    err << child("rhs")->trace(instance, ACT_READ);
    err << child("lhs")->trace(instance, ACT_WRITE);
  }
  else {
    err << child("rhs")->trace(instance, ACT_READ);
    err << child("lhs")->trace(instance, ACT_READ);
    err << child("lhs")->trace(instance, ACT_WRITE);
  }

  return Err();
}

//------------------------------------------------------------------------------

Err CNodeCompound::trace(CInstance* instance, TraceAction action) {
  Err err;
  for (auto c : this) err << c->trace(instance, action);
  return err;
}

//------------------------------------------------------------------------------

Err CNodeReturn::trace(CInstance* instance, TraceAction action) {
  Err err;
  if (auto value = child("value")) {
    err << value->trace(instance, action);
  }
  return err;
}

//------------------------------------------------------------------------------
