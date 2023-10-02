#pragma once

#include "metron/nodes/CNodeStatement.hpp"

struct CNodeClass;
struct CNodeFunction;
struct CNodeLValue;

//==============================================================================

struct CNodeAssignment : public CNodeStatement {
  void init() {
    node_lhs = child("lhs")->req<CNodeLValue>();
    node_op = child("op");
    node_rhs = child("rhs");
  }

  CNodeLValue* node_lhs = nullptr;
  CNode* node_op = nullptr;
  CNode* node_rhs = nullptr;
};

//==============================================================================
