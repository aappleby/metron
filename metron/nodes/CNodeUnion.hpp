#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeUnion : public CNode {
};

//==============================================================================

struct CNodeUnionType : public CNodeType {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);
    name = child("name")->name;
  }
};

//==============================================================================
