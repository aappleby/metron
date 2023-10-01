#pragma once

#include "metron/nodes/CNodeStatement.hpp"

//==============================================================================

struct CNodeExpStatement : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);
    node_exp = child("exp")->req<CNode>();
  }

  CNode* node_exp = nullptr;
};

//==============================================================================
