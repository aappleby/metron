#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeIdentifier : public CNode {
  void init() {
    color = 0x80FF80;
    name = get_text();
  }
};

//==============================================================================
