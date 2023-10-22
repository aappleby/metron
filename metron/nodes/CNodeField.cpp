#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeClass.hpp"

#include "metron/CSourceRepo.hpp"

CNodeClass* CNodeField::get_type_class() {
  return get_repo()->get_class(node_decl->node_type->name);
}

CNodeStruct* CNodeField::get_type_struct() {
  return get_repo()->get_struct(node_decl->node_type->name);
}
