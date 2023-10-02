#pragma once

#include "metron/nodes/CNodeStatement.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodePunct.hpp"

//==============================================================================

struct CNodeReturn : public CNodeStatement {
  void init() {
    node_ret  = child("return")->req<CNodeKeyword>();
    node_val  = child("value");
    node_semi = child("semi")->req<CNodePunct>();
  }


  CNodeKeyword* node_ret  = nullptr;
  CNode* node_val  = nullptr;
  CNodePunct* node_semi = nullptr;
};

//==============================================================================
