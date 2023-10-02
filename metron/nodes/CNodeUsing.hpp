#pragma once

#include "metron/nodes/CNodeStatement.hpp"

//==============================================================================

struct CNodeUsing : public CNodeStatement {
  void init() {
    color = 0x00DFFF;
    name = child("name")->name;
  }
};

//==============================================================================
