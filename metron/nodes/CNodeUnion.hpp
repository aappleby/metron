#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeUnion : public CNode {
};

//==============================================================================

struct CNodeUnionType : public CNodeType {
  std::string_view get_name() const override {
    return child("name")->get_text();
  }
};

//==============================================================================
