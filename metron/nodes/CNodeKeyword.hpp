#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeKeyword : public CNode {
  void init() {
    name = get_text();
  }
};

//==============================================================================
