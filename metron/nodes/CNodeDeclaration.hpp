#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeType.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodePunct.hpp"

//==============================================================================

struct CNodeDeclaration : public CNode {
  void init() {
    node_type   = child("type")->as<CNodeType>();
    node_name   = child("name")->as<CNodeIdentifier>();
    node_array  = child("array")->as<CNodeList>();
    node_eq     = child("eq")->as<CNodePunct>();
    node_value  = child("value")->as<CNode>();

    name = node_name->name;
  }

  bool is_component() const { return _type_class != nullptr; }
  bool is_struct() const { return _type_struct != nullptr; }
  bool is_array() const { return node_array != nullptr; }
  bool is_param() const {
    return node_type->is_param();
  }

  bool is_const_char() const {
    if (is_param()) {
      auto builtin = node_type->node_name;
      auto star = node_type->node_star;
      if (builtin && star && builtin->get_text() == "char") {
        return true;
      }
    }
    return false;
  }

  CNodeType*       node_type = nullptr;
  CNodeIdentifier* node_name = nullptr;
  CNodeList*       node_array = nullptr;
  CNodePunct*      node_eq = nullptr;
  CNode*           node_value = nullptr;

  CNodeClass*  _type_class = nullptr;
  CNodeStruct* _type_struct = nullptr;
};

//==============================================================================
