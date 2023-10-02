#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodePunct.hpp"

//==============================================================================

struct CNodeList : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);
    for (auto child : this) {
      if (!child->as<CNodePunct>()) items.push_back(child);
    }
    color = 0xCCCCCC;
    name = "<CNodeList>";
  }

  std::vector<CNode*> items;
};

//==============================================================================
