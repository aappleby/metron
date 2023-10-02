#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeList.hpp"

//------------------------------------------------------------------------------

struct CNodeCall : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);
    node_name  = child("func_name")->req<CNode>();
    node_targs = child("func_targs")->opt<CNodeList>();
    node_args  = child("func_args")->req<CNodeList>();
    name = child("func_name")->get_text();
    color = COL_SKY;
  }

  CNode* node_name = nullptr;
  CNodeList* node_targs = nullptr;
  CNodeList* node_args = nullptr;
};

//------------------------------------------------------------------------------

struct CNodeArgument : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);
    // FIXME
    name = "arg";
  }
};

//------------------------------------------------------------------------------
