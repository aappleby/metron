#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeExpression.hpp"

struct CNodeClass;
struct CNodeStruct;
struct CNodeUnion;

//------------------------------------------------------------------------------

struct CNodeField : public CNode {
  void init() {
    node_decl = child("decl")->req<CNodeDeclaration>();
    node_bits = child("bits")->opt<CNodeConstInt>();
    node_semi = child("semi")->req<CNodePunct>();
    name = node_decl->name;
  }

  bool is_component()  const { return node_decl->is_component(); }
  bool is_struct()     const { return node_decl->is_struct(); }
  bool is_union()      const { return node_decl->is_union(); }
  bool is_array()      const { return node_decl->is_array(); }
  bool is_const_char() const { return node_decl->is_const_char(); }

  CNodeClass*  get_type_class();
  CNodeStruct* get_type_struct();
  CNodeUnion*  get_type_union();

  //----------------------------------------

  bool is_public = false;

  CNodeDeclaration* node_decl;
  CNodeConstInt* node_bits;
  CNodePunct* node_semi;
};

//------------------------------------------------------------------------------
