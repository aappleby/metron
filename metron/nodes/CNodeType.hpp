#pragma once

#include "metron/CNode.hpp"
#include "metron/Cursor.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodeKeyword.hpp"

//==============================================================================

struct CNodeType : public CNode {
  virtual void init() {
    node_static = child("static")->as<CNodeKeyword>();
    node_const  = child("const")->as<CNodeKeyword>();
    node_name   = child("name");
    node_star   = child("star");
    node_targs  = child("template_args")->as<CNodeList>();
    node_scope  = child("scope");

    name = node_name->name;
  }

  bool is_param() {
    return node_static != nullptr && node_const != nullptr;
  }

  CNodeKeyword* node_static = nullptr;
  CNodeKeyword* node_const = nullptr;
  CNode*        node_name = nullptr;
  CNode*        node_star = nullptr;
  CNodeList*    node_targs = nullptr;
  CNode*        node_scope = nullptr;
};

struct CNodeBuiltinType : public CNodeType {};
struct CNodeClassType : public CNodeType {};
struct CNodeEnumType : public CNodeType {};
struct CNodeStructType : public CNodeType {};
struct CNodeTypedefType : public CNodeType {};
struct CNodeUnionType : public CNodeType {};

//==============================================================================
