#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodePunct.hpp"

//==============================================================================

struct CNodeQualifiedIdentifier : public CNode {
  void init() {
    node_scope = child("scope_path");
    node_colon = child("colon")->req<CNodePunct>();
    node_name  = child("identifier");
    color = 0x80FF80;
    name = get_text();
  }

  CNode* node_scope = nullptr;
  CNodePunct* node_colon = nullptr;
  CNode* node_name  = nullptr;
};

//==============================================================================
