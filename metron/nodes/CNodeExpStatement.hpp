#pragma once

#include "metron/nodes/CNodeStatement.hpp"

//==============================================================================

struct CNodeExpStatement : public CNodeStatement {
  void init() {
    node_exp = child("exp")->opt<CNode>();
  }

  CNode* node_exp = nullptr;
};

//==============================================================================
