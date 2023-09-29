#pragma once

#include "metron/nodes/CNodeStatement.hpp"

//==============================================================================

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

//==============================================================================
