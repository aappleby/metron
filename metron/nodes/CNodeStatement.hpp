#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeStatement : public CNode {
  uint32_t debug_color() const override {
    return COL_TEAL;
  }
};

//==============================================================================
