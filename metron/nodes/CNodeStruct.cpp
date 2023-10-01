#include "metron/nodes/CNodeStruct.hpp"

#include "metron/Emitter.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodePunct.hpp"

//==============================================================================

void CNodeStruct::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);
  node_struct = child("struct")->req<CNodeKeyword>();
  node_name   = child("name");
  node_body   = child("body")->req<CNodeList>();
  node_semi   = child("semi")->req<CNodePunct>();
}

uint32_t CNodeStruct::debug_color() const {
  return 0xFFAAAA;
}

//------------------------------------------------------------------------------

std::string_view CNodeStruct::get_name() const {
  return child("name")->get_name();
}

//------------------------------------------------------------------------------

Err CNodeStruct::collect_fields_and_methods() {
  Err err;

  auto body = child("body");
  for (auto c : body) {
    if (auto n = c->as<CNodeField>()) {
      n->parent_struct = n->ancestor<CNodeStruct>();
      n->node_decl->_type_struct  = repo->get_struct(n->get_type_name());
      all_fields.push_back(n);
    }
  }

  return err;
}

//==============================================================================

std::string_view CNodeStructType::get_name() const {
  return child("name")->get_text();
}

//==============================================================================
