#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"

//==============================================================================

struct CNodeFieldExpression : public CNode {
  void init() {
    node_path = child("field_path")->as<CNodeIdentifier>();
    node_name = child("identifier")->as<CNodeIdentifier>();
    color = 0x80FF80;
    name = node_name->name;
  }

  CNodeIdentifier* node_path = nullptr;
  CNodeIdentifier* node_name = nullptr;
};

//==============================================================================
