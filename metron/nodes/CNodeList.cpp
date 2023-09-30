#include "CNodeList.hpp"

#include "metron/Emitter.hpp"
#include "metron/nodes/CNodePunct.hpp"

//==============================================================================

void CNodeList::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);
  for (auto child : this) {
    if (!child->as<CNodePunct>()) items.push_back(child);
  }
}

uint32_t CNodeList::debug_color() const { return 0xCCCCCC; }

std::string_view CNodeList::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeList::trace(CInstance* inst, call_stack& stack) {
  Err err;
  for (auto child : this) {
    err << child->trace(inst, stack);
  }
  return err;
}

//==============================================================================
