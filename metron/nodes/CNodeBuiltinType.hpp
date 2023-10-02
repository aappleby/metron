#pragma once

#include "metron/nodes/CNodeType.hpp"

//==============================================================================

struct CNodeBuiltinType : public CNodeType {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);
    name = get_textstr();
  }
};

//==============================================================================
