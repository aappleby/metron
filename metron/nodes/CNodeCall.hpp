#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeList.hpp"

//------------------------------------------------------------------------------

struct CNodeCall : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    node_name  = child("func_name")->req<CNode>();
    node_targs = child("func_targs")->opt<CNodeList>();
    node_args  = child("func_args")->req<CNodeList>();
    color = COL_SKY;
  }

  std::string_view get_name() const override {
    return child("func_name")->get_text();
  }

  CNode* node_name = nullptr;
  CNodeList* node_targs = nullptr;
  CNodeList* node_args = nullptr;
};

//------------------------------------------------------------------------------

struct CNodeArgument : public CNode {
  std::string_view get_name() const override { return "arg"; }
};

//------------------------------------------------------------------------------
