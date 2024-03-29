#pragma once

#include <assert.h>

#include <vector>
#include <set>

#include "metron/CNode.hpp"
#include "metron/Cursor.hpp"

#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodeEnum.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeEnum.hpp"

struct CNodeField;
struct CNodeFunction;
struct CNodeDeclaration;
struct CNodeEnum;
struct CNodeKeyword;
struct CNodeIdentifier;
struct CNodeList;
struct CNodeConstructor;
struct CInstance;
struct CInstClass;

//==============================================================================

struct CNodeClass : public CNode {
  void init() {
    node_kwclass = child("class")->as<CNodeKeyword>();
    node_name  = child("name")->as<CNodeIdentifier>();
    node_body  = child("body")->as<CNodeList>();
    node_semi  = child("semi")->as<CNodePunct>();

    name = node_name->name;

    for (auto child : node_body->items) {
      if (auto node_enum = child->as<CNodeEnum>()) {
        all_enums.push_back(node_enum);
      }
    }
  }

  CNodeField* get_field(std::string_view name) {
    for (auto f : all_fields) if (f->name == name) return f;
    return nullptr;
  }

  CNodeFunction* get_function(std::string_view name) {
    for (auto m : all_functions) if (m->name == name) return m;
    return nullptr;
  }

  CNodeDeclaration* get_modparam(std::string_view name) {
    for (auto p : all_modparams) if (p->name == name) return p;
    return nullptr;
  }

  CNodeEnum* get_enum(std::string_view name) {
    for (auto e : all_enums) if (e->name == name) return e;
    return nullptr;
  }

  CNode* resolve(std::vector<CNode*> path);

  //----------------------------------------

  CNodeKeyword*    node_kwclass = nullptr;
  CNodeIdentifier* node_name = nullptr;
  CNodeList*       node_body = nullptr;
  CNodePunct*      node_semi = nullptr;

  int refcount = 0;

  CInstClass* instance = nullptr;

  CNodeConstructor* constructor = nullptr;
  std::vector<CNodeFunction*>    all_functions;
  std::vector<CNodeField*>       all_fields;
  std::vector<CNodeDeclaration*> all_modparams;
  std::vector<CNodeField*>       all_localparams;
  std::vector<CNodeEnum*>        all_enums;

  std::vector<CNodeFunction*>    sorted_functions;

  std::set<CInstance*> self_reads;
  std::set<CInstance*> self_writes;

  std::set<CNodeField*> input_sigs;
  std::set<CNodeField*> output_sigs;
  std::set<CNodeField*> output_regs;
};

//==============================================================================
