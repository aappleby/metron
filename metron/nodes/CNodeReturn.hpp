#pragma once

#include "metron/nodes/CNodeStatement.hpp"

struct CNodeKeyword;
struct CNodePunct;

//==============================================================================

struct CNodeReturn : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags);
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;

  CNodeKeyword* node_ret  = nullptr;
  CNode* node_val  = nullptr;
  CNodePunct* node_semi = nullptr;
};

//==============================================================================
