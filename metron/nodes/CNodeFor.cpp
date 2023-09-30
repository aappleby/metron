#include "CNodeFor.hpp"

#include "metron/Cursor.hpp"

//==============================================================================

CHECK_RETURN Err CNodeFor::trace(CInstance* inst, call_stack& stack) {
  Err err;
  err << child("init")->trace(inst, stack);
  err << child("condition")->trace(inst, stack);
  err << child("body")->trace(inst, stack);
  err << child("step")->trace(inst, stack);
  return err;
}

CHECK_RETURN Err CNodeFor::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  for (auto child : this) {
    err << cursor.emit(child);
    if (child->node_next) err << cursor.emit_gap();
  }

  return err << cursor.check_done(this);
}

//==============================================================================
