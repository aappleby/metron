#include "CNodeField.hpp"

#include "metrolib/core/Log.h"
#include "metron/CInstance.hpp"
#include "metron/Emitter.hpp"
#include "metron/nodes/CNodeBuiltinType.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeConstructor.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodeNamespace.hpp"
#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeStruct.hpp"
#include "metron/nodes/CNodeTemplate.hpp"
#include "metron/nodes/CNodeType.hpp"

//------------------------------------------------------------------------------

void CNodeField::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);
  node_decl = child("decl")->req<CNodeDeclaration>();
  node_semi = child("semi")->req<CNodePunct>();
}

//------------------------------------------------------------------------------

uint32_t CNodeField::debug_color() const {
  return COL_PINK;
}

std::string_view CNodeField::get_name() const {
  return node_decl->get_name();
}

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

bool CNodeField::is_array() const {
  return node_decl->node_array != nullptr;
}

bool CNodeField::is_const_char() const {
  if (node_decl->node_static && node_decl->node_const) {
    auto builtin = node_decl->node_type->child("name");
    auto star    = node_decl->node_type->child("star");
    if (builtin && star && builtin->get_text() == "char") {
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeField::trace(CInstance* inst, call_stack& stack) {
  return node_decl->trace(inst, stack);
}

//------------------------------------------------------------------------------

void CNodeField::dump() const {
  auto name = get_name();
  LOG_A("Field %.*s : ", name.size(), name.data());

  if (node_decl->node_static) LOG_A("static ");
  if (node_decl->node_const)  LOG_A("const ");
  if (is_public)              LOG_A("public ");
  if (node_decl->is_array())  LOG_A("array ");

  if (parent_class) {
    LOG_A("parent class %s ", parent_class->get_namestr().c_str());
  }

  if (parent_struct) {
    LOG_A("parent struct %s ", parent_struct->get_namestr().c_str());
  }

  if (node_decl->_type_class) {
    LOG_A("type class %s ", node_decl->_type_class->get_namestr().c_str());
  }

  if (node_decl->_type_struct) {
    LOG_A("type struct %s ", node_decl->_type_struct->get_namestr().c_str());
  }

  LOG_A("\n");
}

//------------------------------------------------------------------------------
