#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeList.hpp"

//------------------------------------------------------------------------------

struct CNodeCall : public CNode {
  void init() {
    node_name  = child("func_name")->req<CNode>();
    node_targs = child("func_targs")->opt<CNodeList>();
    node_args  = child("func_args")->req<CNodeList>();

    name  = node_name->name;
    color = COL_SKY;
  }

  CNode* node_name = nullptr;
  CNodeList* node_targs = nullptr;
  CNodeList* node_args = nullptr;
};

//------------------------------------------------------------------------------
