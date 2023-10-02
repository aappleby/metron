#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeKeyword : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);
    color = 0xFFFF88;
    name = get_text();
  }
};

//==============================================================================
