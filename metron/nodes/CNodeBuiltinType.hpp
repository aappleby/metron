#pragma once

#include "metron/nodes/CNodeType.hpp"

//==============================================================================

struct CNodeBuiltinType : public CNodeType {
  void init() {
    name = get_text();
  }
};

//==============================================================================
