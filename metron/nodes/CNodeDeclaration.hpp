#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeType.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodePunct.hpp"

//==============================================================================

struct CNodeDeclaration : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags) {
    CNode::init(match_tag, span, flags);

    color = 0xFF00FF;

    node_static = child("static")->as<CNodeKeyword>();
    node_const  = child("const")->as<CNodeKeyword>();
    node_type   = child("type")->as<CNodeType>();
    node_name   = child("name")->as<CNodeIdentifier>();
    node_array  = child("array")->as<CNodeList>();
    node_eq     = child("eq")->as<CNodePunct>();
    node_value  = child("value")->as<CNode>();
  }

  std::string_view get_name() const override {
    return node_name->get_name();
  }

  std::string_view get_type_name() const {
    return node_type->child_head->get_text();
  }

  bool is_array() const {
    return node_array != nullptr;
  }

  bool is_localparam() const {
    return child("static") != nullptr && child("const") != nullptr;
  }

  CNodeKeyword*    node_static = nullptr;
  CNodeKeyword*    node_const = nullptr;
  CNodeType*       node_type = nullptr;
  CNodeIdentifier* node_name = nullptr;
  CNodeList*       node_array = nullptr;
  CNodePunct*      node_eq = nullptr;
  CNode*           node_value = nullptr;

  CNodeClass*  _type_class = nullptr;
  CNodeStruct* _type_struct = nullptr;
};

//==============================================================================
