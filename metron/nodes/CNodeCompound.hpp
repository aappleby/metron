#pragma once

#include "metron/nodes/CNodeStatement.hpp"
#include "metron/CScope.hpp"

//==============================================================================

struct CNodeCompound : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags);

  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;

  std::vector<CNode*> statements;
  CScope scope;
};

//==============================================================================
