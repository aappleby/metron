#include "CNodeField.hpp"

//------------------------------------------------------------------------------

std::string_view CNodeField::get_name() const {
  return child("decl_name")->get_name();
}

//------------------------------------------------------------------------------

uint32_t CNodeField::debug_color() const {
  return 0xFF00FF;
}

//------------------------------------------------------------------------------

Err CNodeField::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  //dump_tree();

  // Check for const char*
  auto type = child("decl_type");

  bool is_const_char_ptr = false;
  if (child("const")) {
    if (type->child("base_type")->get_text() == "char") {
      if (type->child("star")) {
        is_const_char_ptr = true;
      }
    }
  }

  if (is_const_char_ptr) {
    return cursor.emit_replacement(this, "{{const char*}}");

    /*
    err << cursor.skip_over(child("const"));
    err << cursor.skip_over(child("decl_type"));
    err << cursor.emit_print("localparam string ");
    err << cursor.emit_default(child("decl_name"));
    err << cursor.emit_print(" = ");
    err << cursor.emit_default(child("decl_value"));
    */
  }
  else {
    err << cursor.emit_replacement(this, "{{CNodeField}}");
  }

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------
