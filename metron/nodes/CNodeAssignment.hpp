#pragma once

#include "metron/nodes/CNodeStatement.hpp"
#include "metron/nodes/CNodeExpression.hpp"

struct CNodeClass;
struct CNodeFunction;
struct CNodeLValue;

//==============================================================================

struct CNodeAssignment : public CNodeStatement {
  void init() {
    node_lhs = child("lhs");
    node_op  = child("op")->req<CNodeOperator>();
    node_rhs = child("rhs");
  }

  CNode* node_lhs = nullptr;
  CNodeOperator* node_op = nullptr;
  CNode* node_rhs = nullptr;
};

//==============================================================================
