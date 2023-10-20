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

  bool is_component()  const { return node_decl->is_component(); }
  bool is_struct()     const { return node_decl->is_struct(); }
  bool is_array()      const { return node_decl->is_array(); }
  bool is_const_char() const { return node_decl->is_const_char(); }

  //----------------------------------------

  bool is_public = false;

  CNodeDeclaration* node_decl;
  CNodePunct* node_semi;

  //CNodeClass*  parent_class;
  //CNodeStruct* parent_struct;
};

//------------------------------------------------------------------------------
