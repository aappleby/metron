#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeAccess : public CNode {
  uint32_t debug_color() const override;
  void dump() const override;
};

//==============================================================================
