#pragma once

#include "metron/CNode.hpp"
#include "metron/Cursor.hpp"
#include "metron/nodes/CNodeList.hpp"

//==============================================================================

struct CNodeType : public CNode {
  virtual void init() {
    node_const = child("const");
    node_name  = child("name");
    node_targs = child("template_args")->as<CNodeList>();
    node_scope = child("scope");

    name = node_name->name;
  }

  CNode*     node_const;
  CNode*     node_name;
  CNodeList* node_targs;
  CNode*     node_scope;
};

struct CNodeBuiltinType : public CNodeType {};
struct CNodeClassType : public CNodeType {};
struct CNodeEnumType : public CNodeType {};
struct CNodeStructType : public CNodeType {};
struct CNodeTypedefType : public CNodeType {};
struct CNodeUnionType : public CNodeType {};

//==============================================================================
