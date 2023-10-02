#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeTranslationUnit : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);
    color = 0xFFFF00;
    name = "<CNodeTranslationUnit>";
  }
};

//==============================================================================
