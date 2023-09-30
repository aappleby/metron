#pragma once

#include "metrolib/core/Log.h"
#include "metron/CNode.hpp"
#include "metron/Cursor.hpp"

struct CNodeList;

//------------------------------------------------------------------------------

struct CNodeCall : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags);
  std::string_view get_name() const override;
  uint32_t debug_color() const override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;

  bool is_bit_extract();
  CHECK_RETURN Err emit_bit_extract(Cursor& cursor);

  CNode* node_name = nullptr;
  CNodeList* node_targs = nullptr;
  CNodeList* node_args = nullptr;
};

//------------------------------------------------------------------------------

struct CNodeArgument : public CNode {
  std::string_view get_name() const override { return "arg"; }
};

//------------------------------------------------------------------------------
