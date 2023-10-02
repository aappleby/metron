#pragma once

#include "metron/nodes/CNodeType.hpp"

//==============================================================================

struct CNodeBuiltinType : public CNodeType {
  void init() override {
    name = get_text();
  }
};

//==============================================================================
