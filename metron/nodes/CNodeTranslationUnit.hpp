#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeTranslationUnit : public CNode {
  void init() {
    color = 0xFFFF00;
    name = "<CNodeTranslationUnit>";
  }
};

//==============================================================================
