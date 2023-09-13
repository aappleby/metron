#pragma once

#include "metrolib/core/Log.h"

#include "CNode.hpp"
#include "Cursor.hpp"

#include "NodeTypes.hpp"

//------------------------------------------------------------------------------

struct CNodeCall : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags);
  std::string_view get_name() const override;
  uint32_t debug_color() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CCall* call) override;

  CNode*     node_name  = nullptr;
  CNodeList* node_targs = nullptr;
  CNodeList* node_args  = nullptr;
};

//------------------------------------------------------------------------------

struct CNodeArgument : public CNode {
  std::string_view get_name() const override { return "arg"; }
};

//------------------------------------------------------------------------------
