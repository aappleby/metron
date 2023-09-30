#include "CNodeReturn.hpp"

#include "metron/CInstance.hpp"
#include "metron/Emitter.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodePunct.hpp"

//==============================================================================

void CNodeReturn::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  node_ret  = child("return")->req<CNodeKeyword>();
  node_val  = child("value");
  node_semi = child("semi")->req<CNodePunct>();
}

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
