#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodePunct.hpp"

struct CNodeClass;
struct CNodeStruct;

//------------------------------------------------------------------------------

struct CNodeField : public CNode {
  void init() {
    node_decl = child("decl")->req<CNodeDeclaration>();
    node_semi = child("semi")->req<CNodePunct>();
    name = node_decl->name;
  }

  bool is_component() const { return node_decl->_type_class != nullptr; }

  bool is_struct() const { return node_decl->_type_struct != nullptr; }

  bool is_array() const { return node_decl->node_array != nullptr; }

  bool is_const_char() const {
    if (node_decl->node_static && node_decl->node_const) {
      auto builtin = node_decl->node_type->child("name");
      auto star = node_decl->node_type->child("star");
      if (builtin && star && builtin->get_text() == "char") {
        return true;
      }
    }
    return false;
  }

  //----------------------------------------

  bool is_public = false;

  CNodeDeclaration* node_decl;
  CNodePunct* node_semi;

  CNodeClass* parent_class;
  CNodeStruct* parent_struct;
};

//------------------------------------------------------------------------------
