#pragma once

#include "metron/CNode.hpp"
#include "metron/Cursor.hpp"

//------------------------------------------------------------------------------

struct CNodeExpression : public CNode {
};

//------------------------------------------------------------------------------

struct CNodeBinaryExp : public CNodeExpression {
};

struct CNodePrefixExp : public CNodeExpression {
};

struct CNodeSuffixExp : public CNodeExpression {
};

struct CNodeAssignExp : public CNodeExpression {
};

struct CNodeIdentifierExp : public CNodeExpression {
  void init() {
    name = get_text();
  }
};

//------------------------------------------------------------------------------

struct CNodeOperator : public CNode {
};

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

struct CNodePrefixOp : public CNodeOperator {
};

struct CNodeSuffixOp : public CNodeOperator {
};

struct CNodeAssignOp : public CNodeOperator {
};

struct CNodeCast : public CNodeOperator {
};

//------------------------------------------------------------------------------
