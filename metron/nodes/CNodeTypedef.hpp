#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeType.hpp"

//==============================================================================

struct CNodeTypedef : public CNode {
  void init() {
    color = 0xFFFF88;
    name = child("name")->name;
  }
};

//==============================================================================

struct CNodeTypedefType : public CNodeType {
  void init() {
    name = child("name")->name;
  }
};

//==============================================================================
