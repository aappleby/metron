#include "CNodeField.hpp"

#include <string>

#include "metrolib/core/Log.h"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeType.hpp"

//------------------------------------------------------------------------------

void CNodeField::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);
  node_decl = child("decl")->req<CNodeDeclaration>();
  node_semi = child("semi")->req<CNodePunct>();
  color = COL_PINK;
}

//------------------------------------------------------------------------------

std::string_view CNodeField::get_name() const { return node_decl->get_name(); }

std::string_view CNodeField::get_type_name() const {
  return node_decl->get_type_name();
}

//------------------------------------------------------------------------------

bool CNodeField::is_component() const {
  return node_decl->_type_class != nullptr;
}

bool CNodeField::is_struct() const {
  return node_decl->_type_struct != nullptr;
}

bool CNodeField::is_array() const { return node_decl->node_array != nullptr; }

bool CNodeField::is_const_char() const {
  if (node_decl->node_static && node_decl->node_const) {
    auto builtin = node_decl->node_type->child("name");
    auto star = node_decl->node_type->child("star");
    if (builtin && star && builtin->get_text() == "char") {
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------
