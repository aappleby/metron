#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodePunct : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  void dump() const override {
    auto text = get_text();
    LOG_B("CNodePunct \"%.*s\"\n", text.size(), text.data());
  }
};

//==============================================================================
