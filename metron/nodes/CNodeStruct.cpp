#include "CNodeStruct.hpp"

#include "metron/CSourceRepo.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodePunct.hpp"

//==============================================================================

void CNodeStruct::init() {
  node_struct = child("struct")->req<CNodeKeyword>();
  node_name   = child("name");
  node_body   = child("body")->req<CNodeList>();
  node_semi   = child("semi")->req<CNodePunct>();
  name = node_name->name;
}

//----------------------------------------

Err CNodeStruct::collect_fields_and_methods(CSourceRepo* repo) {
  Err err;

  auto body = child("body");
  for (auto c : body) {
    if (auto n = c->as<CNodeField>()) {
      //n->parent_struct = n->ancestor<CNodeStruct>();
      //n->node_decl->_type_struct  = repo->get_struct(n->node_decl->node_type->name);
      all_fields.push_back(n);
    }
  }

  return err;
}
//==============================================================================
