#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeUnion : public CNode {
};

//==============================================================================

struct CNodeUnionType : public CNodeType {
  void init() override {
    name = child("name")->name;
  }
};

//==============================================================================
