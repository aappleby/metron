#include "CNodeLValue.hpp"

#include "metron/Emitter.hpp"

//==============================================================================

CHECK_RETURN Err CNodeLValue::trace(CInstance* inst, call_stack& stack) {
  Err err;
  for (auto c : this) {
    err << c->trace(inst, stack);
  }
  return err;
}

//==============================================================================
