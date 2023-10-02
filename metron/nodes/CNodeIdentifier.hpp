#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeIdentifier : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);
    color = 0x80FF80;
    name = get_text();
  }
};

//==============================================================================
