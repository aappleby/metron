#pragma once

#include "metron/nodes/CNodeStatement.hpp"

//==============================================================================

struct CNodeExpStatement : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags);
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;

  CNode* node_exp = nullptr;
};

//==============================================================================
