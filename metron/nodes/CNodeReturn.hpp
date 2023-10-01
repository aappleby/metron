#pragma once

#include "metron/nodes/CNodeStatement.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodePunct.hpp"

//==============================================================================

struct CNodeReturn : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);

    node_ret  = child("return")->req<CNodeKeyword>();
    node_val  = child("value");
    node_semi = child("semi")->req<CNodePunct>();
  }


  CNodeKeyword* node_ret  = nullptr;
  CNode* node_val  = nullptr;
  CNodePunct* node_semi = nullptr;
};

//==============================================================================
