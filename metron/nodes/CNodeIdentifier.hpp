#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeIdentifier : public CNode {
  CNodeIdentifier() {
    color = 0x80FF80;
  }

  std::string_view get_name() const override {
    return get_text();
  }
};

//==============================================================================
