#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodePunct : public CNode {
  uint32_t debug_color() const override { return 0x88FF88; }

  std::string_view get_name() const override {
    NODE_ERR("FIXME");
    return "";
  }
};

//==============================================================================
