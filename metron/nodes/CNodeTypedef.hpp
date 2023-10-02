#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeType.hpp"

//==============================================================================

struct CNodeTypedef : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);
    color = 0xFFFF88;
    name = child("name")->name;
  }
};

//==============================================================================

struct CNodeTypedefType : public CNodeType {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);
    name = child("name")->name;
  }
};

//==============================================================================
