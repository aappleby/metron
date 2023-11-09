#include "CNodeStruct.hpp"

#include "metron/CSourceRepo.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeUnion.hpp"

//==============================================================================

void CNodeStruct::init() {
  node_struct = child("struct")->req<CNodeKeyword>();
  node_name   = child("name");
  node_body   = child("body")->req<CNodeList>();
  node_semi   = child("semi")->req<CNodePunct>();
  name = node_name->name;

  for (auto c : node_body) {
    if (auto n = c->as<CNodeField>()) {
      all_fields.push_back(n);
    }
  }
}

//----------------------------------------

CNodeField* CNodeStruct::get_field(std::string_view name) {
  for (auto f : all_fields) if (f->name == name) return f;
  return nullptr;
}

CNode* CNodeStruct::resolve(std::vector<CNode*> path) {
  auto front = path[0];
  path.erase(path.begin());

  if (path.empty()) {
    if (auto f = get_field(front->name)) return f;
  }

  if (auto f = get_field(front->name)) {
    if (auto s = f->get_type_struct()) {
      return s->resolve(path);
    }
    if (auto u = f->get_type_union()) {
      return u->resolve(path);
    }

    assert(false);
  }

  return nullptr;
}

//==============================================================================
