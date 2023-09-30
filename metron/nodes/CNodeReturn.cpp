#include "CNodeReturn.hpp"

#include "metron/CInstance.hpp"
#include "metron/Emitter.hpp"
#include "metron/nodes/CNodeFunction.hpp"

//==============================================================================

Err CNodeReturn::trace(CInstance* inst, call_stack& stack) {
  Err err;

  if (auto node_value = child("value")) {
    err << node_value->trace(inst, stack);
  }

  auto inst_return = inst->resolve("@return");
  assert(inst_return);
  err << inst_return->log_action(this, ACT_WRITE, stack);

  return err;
}

//==============================================================================
