#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"

//==============================================================================

struct CNodeFieldExpression : public CNode {
  void init(const char* match_tag, SpanType span,
                                  uint64_t flags) {
    CNode::init(match_tag, span, flags);
    node_path = child("field_path")->as<CNodeIdentifier>();
    node_name = child("identifier")->as<CNodeIdentifier>();
    color = 0x80FF80;
  }

  std::string_view get_name() const override {
    NODE_ERR("FIXME");
    return "";
  }

  CNodeIdentifier* node_path = nullptr;
  CNodeIdentifier* node_name = nullptr;
};

//==============================================================================
