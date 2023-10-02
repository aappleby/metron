#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodePunct.hpp"

//==============================================================================

struct CNodeList : public CNode {
  void init() {
    node_ldelim = child("ldelim")->opt<CNodePunct>();
    node_rdelim = child("rdelim")->opt<CNodePunct>();

    for (auto child : this) {
      if (!child->as<CNodePunct>()) items.push_back(child);
    }
    name = "<CNodeList>";
  }

  CNodePunct* node_ldelim = nullptr;
  std::vector<CNode*> items;
  CNodePunct* node_rdelim = nullptr;
};

//==============================================================================
