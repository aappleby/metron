#pragma once

#include "metron/CNode.hpp"

struct CNodeIdentifier;

//==============================================================================

struct CNodeFieldExpression : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags);

  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;

  CNodeIdentifier* node_path = nullptr;
  CNodeIdentifier* node_name = nullptr;
};

//==============================================================================
