#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"
#include "CScope.hpp"
#include "metron/tools/MtUtils.h"

//==============================================================================

#if 0

statement:
  labeled-statement
  compound-statement
  expression-statement
  selection-statement
  iteration-statement
  jump-statement

labeled-statement:
  identifier : statement
  case constant-expression : statement
  default : statement

compound-statement:
  { block-item-listopt }

block-item-list:
  block-item
  block-item-list block-item

block-item:
  declaration
  statement

expression-statement:
  expressionopt ;

selection-statement:
  if ( expression ) statement
  if ( expression ) statement else statement
  switch ( expression ) statement

iteration-statement:
  while ( expression ) statement
  do statement while ( expression ) ;
  for ( expressionopt ; expressionopt ; expressionopt ) statement
  for ( declaration expressionopt ; expressionopt ) statement

jump-statement:
  goto identifier ;
  continue ;
  break ;
  return expressionopt ;

#endif

//==============================================================================

struct CNodeStatement : public CNode {
  uint32_t debug_color() const override;
protected:
  CNodeStatement() {}
};

//------------------------------------------------------------------------------

struct CNodeExpStatement : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags);
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;

  CNode* node_exp = nullptr;
};

struct CNodeAssignment : public CNodeStatement {
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
};

//------------------------------------------------------------------------------

struct CNodeLValue : public CNode {
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
};

//------------------------------------------------------------------------------

struct CNodeUsing : public CNodeStatement {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
};

//------------------------------------------------------------------------------

struct CNodeFor : public CNodeStatement {
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
};

//------------------------------------------------------------------------------

struct CNodeIf : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags);
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  CHECK_RETURN Err emit(Cursor& cursor) override;

  CNodeKeyword*   node_kw_if = nullptr;
  CNodeList*      node_cond = nullptr;
  CNodeStatement* node_then = nullptr;
  CNodeKeyword*   node_kw_else = nullptr;
  CNodeStatement* node_else = nullptr;
};

//------------------------------------------------------------------------------

struct CNodeReturn : public CNodeStatement {
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
};

//------------------------------------------------------------------------------

struct CNodeSwitch : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags);
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  CHECK_RETURN Err emit(Cursor& cursor) override;

  CNodeList* node_condition = nullptr;
  CNodeList* node_body = nullptr;
};

struct CNodeCase : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags);

  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  CHECK_RETURN Err emit(Cursor& cursor) override;

  CNodeKeyword* node_case  = nullptr;
  CNode*        node_cond  = nullptr;
  CNodePunct*   node_colon = nullptr;
  CNodeList*    node_body  = nullptr;
};

struct CNodeDefault : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags);

  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  CHECK_RETURN Err emit(Cursor& cursor) override;

  CNodeKeyword* node_default = nullptr;
  CNodePunct*   node_colon   = nullptr;
  CNodeList*    node_body    = nullptr;
};

//------------------------------------------------------------------------------

struct CNodeDoWhile : public CNodeStatement {
};

struct CNodeWhile : public CNodeStatement {
};

struct CNodeCompound : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags);

  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err emit_block(Cursor& cursor, std::string ldelim, std::string rdelim);
  CHECK_RETURN Err emit_hoisted_decls(Cursor& cursor);
  CHECK_RETURN Err emit_call_arg_bindings(CNode* child, Cursor& cursor);

  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;

  std::vector<CNode*> statements;
  CScope scope;
};

//------------------------------------------------------------------------------
