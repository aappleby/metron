#pragma once

#include "metron/CNode.hpp"
#include "metron/Cursor.hpp"

//------------------------------------------------------------------------------

struct CNodeExpression : public CNode {
  uint32_t debug_color() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;

protected:
  CNodeExpression() {}
};

//------------------------------------------------------------------------------

struct CNodeBinaryExp : public CNodeExpression {
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
};

struct CNodePrefixExp : public CNodeExpression {
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
};

struct CNodeSuffixExp : public CNodeExpression {
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
};

struct CNodeAssignExp : public CNodeExpression {
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
};

struct CNodeIdentifierExp : public CNodeExpression {
  std::string_view get_name() const override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
};

//------------------------------------------------------------------------------

struct CNodeOperator : public CNode {
  uint32_t debug_color() const override;
  Err emit(Cursor& cursor) override;
  Err trace(CInstance* inst, call_stack& stack) override;
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
