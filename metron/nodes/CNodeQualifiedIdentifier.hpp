#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodePunct.hpp"

//==============================================================================

struct CNodeQualifiedIdentifier : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);

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
