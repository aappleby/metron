#pragma once

#include "metron/nodes/CNodeStatement.hpp"

//==============================================================================

struct CNodeUsing : public CNodeStatement {
  void init() {
    name = child("name")->name;
  }
};

//==============================================================================
