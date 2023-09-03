#include "CNodeStruct.hpp"

#include "CNodeField.hpp"
#include "CNodeFunction.hpp"
#include "metrolib/core/Log.h"

struct CSourceRepo;
struct CSourceFile;

//------------------------------------------------------------------------------

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
    if (auto n = c->as_a<CNodeField>()) {
      n->_parent_struct = n->ancestor<CNodeStruct>();
      n->_type_struct  = repo->get_struct(n->get_type_name());
      all_fields.push_back(n);
    }
  }

  return err;
}

//------------------------------------------------------------------------------

/*
  // Struct outside of class
  if (current_mod.top() == nullptr) {
    // sym_field_declaration
    //   field_type : sym_template_type
    //   field_declarator : sym_field_identifier
    //   lit ;

    return err << emit_children(n);
  }
*/

Err CNodeStruct::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_struct = child("struct");
  auto node_name   = child("name");
  auto node_body   = child("body");

  err << cursor.emit_replacement(node_struct, "typedef struct packed");
  err << cursor.emit_gap_after(node_struct);
  err << cursor.skip_over(node_name);
  err << cursor.skip_gap_after(node_name);
  err << cursor.emit_default(node_body);
  err << cursor.emit_print(" ");
  err << cursor.emit_splice(node_name);
  err << cursor.emit_gap_after(node_body);

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

void CNodeStruct::dump() {
  auto name = get_name();
  LOG_B("Struct %.*s @ %p\n", name.size(), name.data(), this);
  LOG_INDENT();

  if (all_fields.size()) {
    for (auto f : all_fields) f->dump();
  }

  LOG_DEDENT();
}

//------------------------------------------------------------------------------
