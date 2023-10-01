#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeTranslationUnit : public CNode {
  uint32_t debug_color() const override { return 0xFFFF00; }

  std::string_view get_name() const override {
    NODE_ERR("FIXME");
    return "";
  }
};

//==============================================================================
