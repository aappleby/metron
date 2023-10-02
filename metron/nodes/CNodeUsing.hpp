#pragma once

#include "metron/nodes/CNodeStatement.hpp"

//==============================================================================

struct CNodeUsing : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);
    color = 0x00DFFF;
    name = child("name")->name;
  }
};

//==============================================================================
