#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeIdentifier : public CNode {
  void init() {
    name = get_text();
  }
};

//==============================================================================
