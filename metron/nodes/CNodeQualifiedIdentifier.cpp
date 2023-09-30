#include "CNodeQualifiedIdentifier.hpp"

#include "metron/Emitter.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeNamespace.hpp"
#include "metron/nodes/CNodeField.hpp"

//==============================================================================

uint32_t CNodeQualifiedIdentifier::debug_color() const { return 0x80FF80; }

//------------------------------------------------------------------------------

std::string_view CNodeQualifiedIdentifier::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

//------------------------------------------------------------------------------

Err CNodeQualifiedIdentifier::trace(CInstance* inst, call_stack& stack) {
  Err err;

  auto scope = child("scope_path")->get_name();
  auto field = child("identifier")->get_name();

  if (auto node_namespace = get_repo()->get_namespace(scope)) {
    return node_namespace->get_field(field)->trace(inst, stack);
  }

  if (auto node_enum = get_repo()->get_enum(scope)) {
    return Err();
  }

  if (auto node_class = ancestor<CNodeClass>()) {
    if (auto node_enum = node_class->get_enum(scope)) {
      return Err();
    }
  }

  NODE_ERR("Don't know how to trace this qualified identifier");

  return err;
}

//==============================================================================
