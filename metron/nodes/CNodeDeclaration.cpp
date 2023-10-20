#include "CNodeDeclaration.hpp"

#include "metron/CSourceRepo.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeType.hpp"
//#include "metron/nodes/CNodeKeyword.hpp"
//#include "metron/nodes/CNodeList.hpp"
//#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeClass.hpp"


void CNodeDeclaration::init() {
  node_type   = child("type")->as<CNodeType>();
  node_name   = child("name")->as<CNodeIdentifier>();
  node_array  = child("array")->as<CNodeList>();
  node_eq     = child("eq")->as<CNodePunct>();
  node_value  = child("value")->as<CNode>();

  name = node_name->name;
}

CNodeStruct* CNodeDeclaration::get_struct() {
  return get_repo()->get_struct(node_type->name);
}

CNodeClass* CNodeDeclaration::get_class() {
  return get_repo()->get_class(node_type->name);
}

bool CNodeDeclaration::is_struct()    { return get_struct() != nullptr; }
bool CNodeDeclaration::is_component() { return get_class() != nullptr; }
bool CNodeDeclaration::is_class()     { return get_class() != nullptr; }
bool CNodeDeclaration::is_array() const { return node_array != nullptr; }
bool CNodeDeclaration::is_param() const { return node_type->is_param(); }

bool CNodeDeclaration::is_const_char() const {
  if (is_param()) {
    auto builtin = node_type->node_name;
    auto star = node_type->node_star;
    if (builtin && star && builtin->get_text() == "char") {
      return true;
    }
  }
  return false;
}
