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
    node_static = child("static")->as<CNodeKeyword>();
    node_const  = child("const")->as<CNodeKeyword>();
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
  bool is_localparam() const {
    return node_static != nullptr && node_const != nullptr;
  }

  bool is_const_char() const {
    if (node_static && node_const) {
      auto builtin = node_type->child("name");
      auto star = node_type->child("star");
      if (builtin && star && builtin->get_text() == "char") {
        return true;
      }
    }
    return false;
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
