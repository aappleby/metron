#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodePreproc : public CNode {
  CNodePreproc() {
    color = 0x00BBBB;
  }

  std::string_view get_name() const override {
    NODE_ERR("FIXME");
    return "";
  }
};

//==============================================================================
