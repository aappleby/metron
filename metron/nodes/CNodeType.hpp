#pragma once

#include "metron/CNode.hpp"
#include "metron/Cursor.hpp"

//==============================================================================

struct CNodeType : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);
    color = COL_VIOLET;
  }
};

//==============================================================================
