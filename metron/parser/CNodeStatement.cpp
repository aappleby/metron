#include "CNodeStatement.hpp"

uint32_t CNodeStatement::debug_color() const {
  return COL_TEAL;
}

Err CNodeStatement::emit(Cursor& cursor) {
  return cursor.emit_default(this);
}

Err CNodeCompound::trace(CInstance* instance, TraceAction action) {
  Err err;
  for (auto c : this) err << c->trace(instance, action);
  return err;
}


Err CNodeReturn::trace(CInstance* instance, TraceAction action) {
  Err err;
  if (auto value = child("value")) {
    err << value->trace(instance, action);
  }
  return err;
}
