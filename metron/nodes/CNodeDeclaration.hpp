#pragma once
#include "metron/CNode.hpp"

struct CNodeStruct;
struct CNodeClass;
struct CNodeType;
struct CNodeIdentifier;
struct CNodeList;
struct CNodePunct;

//==============================================================================

struct CNodeDeclaration : public CNode {
  void init();
  CNodeStruct* get_struct();
  CNodeClass* get_class();
  bool is_struct();
  bool is_component();
  bool is_class();
  bool is_array() const;
  bool is_param() const;
  bool is_const_char() const;

  CNodeType*       node_type = nullptr;
  CNodeIdentifier* node_name = nullptr;
  CNodeList*       node_array = nullptr;
  CNodePunct*      node_eq = nullptr;
  CNode*           node_value = nullptr;

  CNodeClass*  _type_class = nullptr;
  //CNodeStruct* _type_struct = nullptr;
};

//==============================================================================
