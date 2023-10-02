#pragma once

#include "metron/CNode.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeEnum.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeType.hpp"
#include "metron/nodes/CNodeExpression.hpp"

void dump_parse_tree(CNode* node);

//==============================================================================

struct CNodeEnum : public CNode {
  void init() {
    node_enum = child("enum")->as<CNodeKeyword>();
    node_class = child("class")->as<CNodeKeyword>();
    node_name = child("name")->as<CNodeIdentifier>();
    node_colon = child("colon")->as<CNodePunct>();
    node_type = child("base_type")->as<CNodeType>();
    node_body = child("body")->as<CNodeList>();
    node_decl = child("decl");
    node_semi = child("semi")->as<CNodePunct>();

    name = node_name ? node_name->name : "<unnamed>";
  }

  CNodeKeyword* node_enum = nullptr;
  CNodeKeyword* node_class = nullptr;
  CNodeIdentifier* node_name = nullptr;
  CNodePunct* node_colon = nullptr;
  CNodeType* node_type = nullptr;
  CNodeList* node_body = nullptr;
  CNode* node_decl = nullptr;
  CNodePunct* node_semi = nullptr;
};

//==============================================================================

struct CNodeEnumerator : public CNode {
  void init() {
    node_name = child("name")->as<CNodeIdentifier>();
    node_eq = child("eq")->opt<CNodePunct>();
    node_value = child("value")->opt<CNodeConstInt>();
    name = node_name->name;
  }

  CNodeIdentifier* node_name = nullptr;
  CNodePunct* node_eq = nullptr;
  CNodeConstInt* node_value = nullptr;
};

//==============================================================================
