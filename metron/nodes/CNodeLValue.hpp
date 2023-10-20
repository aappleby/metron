#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeLValue : public CNode {
  void init() {
    node_name = child("name");
    node_suffix = child("suffix");
  }

  CNode* node_name = nullptr;
  CNode* node_suffix = nullptr;
};

//==============================================================================
