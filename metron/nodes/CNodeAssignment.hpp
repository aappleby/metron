#pragma once

#include "metron/nodes/CNodeStatement.hpp"

struct CNodeClass;
struct CNodeFunction;
struct CNodeLValue;

//==============================================================================

struct CNodeAssignment : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags);

  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;

  CNodeLValue* node_lhs = nullptr;
  CNode* node_op = nullptr;
  CNode* node_rhs = nullptr;
};

//==============================================================================
