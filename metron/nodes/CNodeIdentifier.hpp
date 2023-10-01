#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeIdentifier : public CNode {
  uint32_t debug_color() const override { return 0x80FF80; }

  std::string_view get_name() const override {
    return get_text();
  }
};

//==============================================================================
