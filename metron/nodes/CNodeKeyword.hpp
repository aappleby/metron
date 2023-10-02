#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeKeyword : public CNode {
  CNodeKeyword() {
    color = 0xFFFF88;
  }

  std::string_view get_name() const override {
    NODE_ERR("FIXME");
    return "";
  }
};

//==============================================================================
