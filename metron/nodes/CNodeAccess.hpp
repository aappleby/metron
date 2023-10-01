#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeAccess : public CNode {
  uint32_t debug_color() const override { return COL_VIOLET; }

  void dump() const override {
    auto text = get_text();
    LOG_B("CNodeAccess \"%.*s\"\n", text.size(), text.data());
  }
};

//==============================================================================
