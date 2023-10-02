#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeType.hpp"

//==============================================================================

struct CNodeTypedef : public CNode {
  CNodeTypedef() {
    color = 0xFFFF88;
  }

  std::string_view get_name() const override {
    NODE_ERR("FIXME");
    return "";
  }
};

//==============================================================================

struct CNodeTypedefType : public CNodeType {
  std::string_view get_name() const override {
    return child("name")->get_text();
  }
};

//==============================================================================
