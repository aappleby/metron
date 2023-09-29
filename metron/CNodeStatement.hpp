#pragma once

#include "metron/CNode.hpp"
#include "metron/Cursor.hpp"
#include "metron/CScope.hpp"
#include "metron/MtUtils.h"

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

//------------------------------------------------------------------------------

struct CNodeCase : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags);

  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  CHECK_RETURN Err emit(Cursor& cursor) override;

  CNodeKeyword* node_case  = nullptr;
  CNode*        node_cond  = nullptr;
  CNodePunct*   node_colon = nullptr;
  CNodeList*    node_body  = nullptr;
};

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

struct CNodeWhile : public CNodeStatement {
};

//------------------------------------------------------------------------------

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
