#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

// CNodeExpression
//   CNodeConstant
//     CNodeConstInt
//     CNodeConstFloat
//     CNodeConstChar
//     CNodeConstString
//   CNodeBinaryExp
//   CNodePrefixExp
//   CNodeSuffixExp
//   CNodeAssignExp

// CNodeOperator
//   CNodeBinaryOp
//   CNodePrefixOp
//   CNodeSuffixOp
//   CNodeCast

//------------------------------------------------------------------------------

struct CNodeExpression : public CNode {
  virtual uint32_t debug_color() const override;
  virtual Err emit(Cursor& cursor) override;

  bool is_integer_constant();

protected:
  CNodeExpression() {}
};

//------------------------------------------------------------------------------

struct CNodeBinaryExp : public CNodeExpression {
  virtual Err trace(IContext* context) override;
};

struct CNodePrefixExp : public CNodeExpression {
  virtual Err trace(IContext* context) override;
};

struct CNodeSuffixExp : public CNodeExpression {
  virtual Err trace(IContext* context) override;
};

struct CNodeAssignExp : public CNodeExpression {
  virtual Err trace(IContext* context) override;
};

struct CNodeIdentifierExp : public CNodeExpression {
  virtual Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodeConstant : public CNodeExpression {
  virtual Err emit(Cursor& cursor) override;
  virtual Err trace(IContext* context) override;

protected:
  CNodeConstant() {}
};

struct CNodeConstInt    : public CNodeConstant {};
struct CNodeConstFloat  : public CNodeConstant {};
struct CNodeConstChar   : public CNodeConstant {};
struct CNodeConstString : public CNodeConstant {};

//------------------------------------------------------------------------------

struct CNodeOperator : public CNode {
  uint32_t debug_color() const override;
  Err emit(Cursor& cursor) override;
  Err trace(IContext* context) override;
};

//----------------------------------------

struct CNodeBinaryOp : public CNodeOperator {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);

    // FIXME this is silly
    char buf[16] = {0};
    memcpy(buf, span.begin->lex->text_begin, span.begin->lex->len());
    precedence = binary_precedence(buf);
    assoc = binary_assoc(buf);
  }

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
