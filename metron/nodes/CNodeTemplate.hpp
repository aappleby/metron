#pragma once

#include <vector>
#include "metron/CNode.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeKeyword.hpp"

struct CNodeKeyword;
struct CNodeList;
struct CNodeClass;
struct CNodeDeclaration;

//------------------------------------------------------------------------------

struct CNodeTemplate : public CNode {
  void init() {
    node_template = child("template")->as<CNodeKeyword>();
    node_params   = child("params")->as<CNodeList>();
    node_class    = child("class")->as<CNodeClass>();

    for (auto child : node_params->items) {
      auto decl = child->as<CNodeDeclaration>();
      assert(decl);
      params.push_back(decl);
    }
  }

  CNodeKeyword* node_template = nullptr;
  CNodeList*    node_params = nullptr;
  CNodeClass*   node_class = nullptr;

  std::vector<CNodeDeclaration*> params;
};

//------------------------------------------------------------------------------
