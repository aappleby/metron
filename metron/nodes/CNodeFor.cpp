#include "CNodeFor.hpp"

#include "metron/Emitter.hpp"

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
  return Emitter(cursor).emit(this);
}

//==============================================================================
