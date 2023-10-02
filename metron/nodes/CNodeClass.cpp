#include "CNodeClass.hpp"

#include "metron/Emitter.hpp"

#include "metron/nodes/CNodeAccess.hpp"
#include "metron/nodes/CNodeCall.hpp"
#include "metron/nodes/CNodeConstructor.hpp"
#include "metron/nodes/CNodeEnum.hpp"
#include "metron/nodes/CNodeExpression.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeFieldExpression.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeLValue.hpp"
#include "metron/nodes/CNodeTemplate.hpp"

//==============================================================================

void CNodeClass::init() {
  node_class = child("class")->as<CNodeKeyword>();
  node_name  = child("name")->as<CNodeIdentifier>();
  node_body  = child("body")->as<CNodeList>();

  name = node_name->get_namestr();
  color = 0x00FF00;

  for (auto child : node_body->items) {
    if (auto node_enum = child->as<CNodeEnum>()) {
      all_enums2.push_back(node_enum);
    }
  }
}

CNodeField* CNodeClass::get_field(std::string_view name) {
  for (auto f : all_fields) if (f->name == name) return f;
  return nullptr;
}

CNodeFunction* CNodeClass::get_function(std::string_view name) {
  for (auto m : all_functions) if (m->name == name) return m;
  return nullptr;
}

CNodeDeclaration* CNodeClass::get_modparam(std::string_view name) {
  for (auto p : all_modparams) if (p->name == name) return p;
  return nullptr;
}

CNodeEnum* CNodeClass::get_enum(std::string_view name) {
  for (auto e : all_enums2) if (e->name == name) return e;
  return nullptr;
}

//==============================================================================
