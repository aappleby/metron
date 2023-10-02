#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeType.hpp"

//==============================================================================

struct CNodeTypedef : public CNode {
  void init() {
    name = child("name")->name;
  }
};

//==============================================================================

struct CNodeTypedefType : public CNodeType {
  void init() override {
    name = child("name")->name;
  }
};

//==============================================================================
