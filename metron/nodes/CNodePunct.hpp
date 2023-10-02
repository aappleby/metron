#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodePunct : public CNode {
  CNodePunct() {
    color = 0x88FF88;
  }

  std::string_view get_name() const override {
    NODE_ERR("FIXME");
    return "";
  }
};

//==============================================================================
