#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodePunct : public CNode {
  uint32_t debug_color() const override { return 0x88FF88; }

  std::string_view get_name() const override {
    NODE_ERR("FIXME");
    return "";
  }

  void dump() const override {
    auto text = get_text();
    LOG_B("CNodePunct \"%.*s\"\n", text.size(), text.data());
  }
};

//==============================================================================
