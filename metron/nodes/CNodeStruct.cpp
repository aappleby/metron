#include "metron/nodes/CNodeStruct.hpp"

#include "metron/Emitter.hpp"
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
  return Emitter(cursor).emit(this);
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
