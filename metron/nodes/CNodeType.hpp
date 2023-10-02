#pragma once

#include "metron/CNode.hpp"
#include "metron/Cursor.hpp"
#include "metron/nodes/CNodeList.hpp"

//==============================================================================

struct CNodeType : public CNode {
  void init() {
    node_targs = child("template_args")->as<CNodeList>();
  }

  CNodeList* node_targs;
};

//==============================================================================
