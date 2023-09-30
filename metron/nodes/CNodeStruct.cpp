#include "metron/nodes/CNodeStruct.hpp"

#include "metron/Cursor.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"

//==============================================================================

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

//------------------------------------------------------------------------------

Err CNodeStruct::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_struct = child("struct");
  auto node_name   = child("name");
  auto node_body   = child("body");
  auto node_semi   = child("semi");

  err << cursor.emit_replacement(node_struct, "typedef struct packed");
  err << cursor.emit_gap();
  err << cursor.skip_over(node_name);
  err << cursor.skip_gap();
  err << cursor.emit_default(node_body);
  err << cursor.emit_print(" ");
  err << cursor.emit_splice(node_name);
  err << cursor.emit_gap();
  err << cursor.emit(node_semi);

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

void CNodeStruct::dump() const {
  auto name = get_name();
  LOG_B("Struct %.*s @ %p\n", name.size(), name.data(), this);
  LOG_INDENT();

  if (all_fields.size()) {
    for (auto f : all_fields) f->dump();
  }

  LOG_DEDENT();
}

//==============================================================================

std::string_view CNodeStructType::get_name() const {
  return child("name")->get_text();
}

CHECK_RETURN Err CNodeStructType::emit(Cursor& cursor) {
  return cursor.emit_default(this);
}

//==============================================================================
