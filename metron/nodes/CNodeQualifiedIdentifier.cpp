#include "CNodeQualifiedIdentifier.hpp"

#include "metron/Cursor.hpp"
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

Err CNodeQualifiedIdentifier::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_scope = child("scope_path");
  auto node_colon = child("colon");
  auto node_name  = child("identifier");

  bool elide_scope = false;

  if (node_scope->get_text() == "std") elide_scope = true;

  auto node_class = ancestor<CNodeClass>();

  // Verilog doesn't like the scope on the enums
  if (node_class && node_class->get_enum(node_scope->get_text())) {
    elide_scope = true;
  }

  if (elide_scope) {
    err << cursor.skip_to(node_name);
    err << cursor.emit(node_name);
  }
  else {
    err << cursor.emit(node_scope);
    err << cursor.emit_gap();
    err << cursor.emit(node_colon);
    err << cursor.emit_gap();
    err << cursor.emit(node_name);
  }

  return err << cursor.check_done(this);
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
