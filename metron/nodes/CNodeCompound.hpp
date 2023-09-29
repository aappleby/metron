#pragma once

#include "metron/nodes/CNodeStatement.hpp"

//==============================================================================

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

//==============================================================================
