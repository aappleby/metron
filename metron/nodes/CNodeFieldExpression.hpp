#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include <vector>

//==============================================================================

struct CNodeFieldExpression : public CNode {
  void init() {
    //node_path = child("field_path")->as<CNodeIdentifier>();
    //node_name = child("identifier")->as<CNodeIdentifier>();
    //name = node_name->name;
    for (auto child : this) {
      if (!child->as<CNodePunct>()) items.push_back(child);
    }
    name = "<CNodeFieldExpression>";
  }

  std::vector<CNode*> items;

  //CNodeIdentifier* node_path = nullptr;
  //CNodeIdentifier* node_name = nullptr;
};

//==============================================================================
