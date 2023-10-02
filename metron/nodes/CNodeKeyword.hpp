#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeKeyword : public CNode {
  void init() {
    color = 0xFFFF88;
    name = get_text();
  }
};

//==============================================================================
