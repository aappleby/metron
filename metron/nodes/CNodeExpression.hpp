#pragma once

#include "metron/CNode.hpp"
#include "metron/Cursor.hpp"

//------------------------------------------------------------------------------

struct CNodeExpression : public CNode {};

struct CNodeConstant : public CNodeExpression {};

struct CNodeConstInt : public CNodeConstant {
  void init() {
    value = atoi(text_begin());
  }
  int value;
};

struct CNodeConstFloat : public CNodeConstant {
  void init() {
    value = atof(text_begin());
  }
  float value;
};

struct CNodeConstChar : public CNodeConstant {};

struct CNodeConstString : public CNodeConstant {};

//------------------------------------------------------------------------------

struct CNodeBinaryExp : public CNodeExpression {};

struct CNodePrefixExp : public CNodeExpression {
  void init() {
    node_prefix = child("prefix")->req<CNode>();
    node_rhs = child("rhs")->req<CNode>();
  }
  CNode* node_prefix;
  CNode* node_rhs;
};

struct CNodeSuffixExp : public CNodeExpression {
  void init() {
    node_lhs = child("lhs")->req<CNode>();
    node_suffix = child("suffix")->req<CNode>();
  }
  CNode* node_lhs;
  CNode* node_suffix;
};

struct CNodeAssignExp : public CNodeExpression {};
struct CNodeIdentifierExp : public CNodeExpression {
  void init() { name = get_text(); }
};

//------------------------------------------------------------------------------

struct CNodeOperator : public CNode {};

//----------------------------------------

struct CNodeBinaryOp : public CNodeOperator {
  void init() {
    // FIXME this is silly
    char buf[16] = {0};
    memcpy(buf, span.begin->lex->text_begin, span.begin->lex->len());
    precedence = binary_precedence(buf);
    assoc = binary_assoc(buf);
  }

  //----------------------------------------
  // -2 = prefix, -1 = right-to-left, 0 = none, 1 = left-to-right, 2 = suffix
  int assoc = 0;
  int precedence = -1;
};

struct CNodePrefixOp : public CNodeOperator {};
struct CNodeSuffixOp : public CNodeOperator {};
struct CNodeAssignOp : public CNodeOperator {};
struct CNodeCast : public CNodeOperator {};

//------------------------------------------------------------------------------
