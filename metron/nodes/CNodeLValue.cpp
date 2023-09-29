#include "CNodeLValue.hpp"

//==============================================================================

CHECK_RETURN Err CNodeLValue::trace(CInstance* inst, call_stack& stack) {
  Err err;
  for (auto c : this) {
    err << c->trace(inst, stack);
  }
  return err;
}

CHECK_RETURN Err CNodeLValue::emit(Cursor& cursor) {
  return cursor.emit_default(this);
}

//==============================================================================
