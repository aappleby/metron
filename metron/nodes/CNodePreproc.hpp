#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodePreproc : public CNode {
  void init() {
    color = 0x00BBBB;
    name = "<CNodePreproc>";
  }
};

//==============================================================================
