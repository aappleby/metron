#include "CNodeDeclaration.hpp"

#include "CNodeType.hpp"

uint32_t CNodeDeclaration::debug_color() const { return 0xFF00FF; }

std::string_view CNodeDeclaration::get_name() const {
  return child("name")->get_name();
}

std::string_view CNodeDeclaration::get_type_name() const {
  auto decl_type = child<CNodeType>();
  return decl_type->child_head->get_text();
}

bool CNodeDeclaration::is_array() const {
  return child("array") != nullptr;
}

//------------------------------------------------------------------------------

Err CNodeDeclaration::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  // Check for const char*
  auto type = child<CNodeType>();

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
  }
  else {
    err << CNode::emit(cursor);
  }

  return err << cursor.check_done(this);
}

CHECK_RETURN Err CNodeDeclaration::trace(IContext* context) {
  Err err;
  if (auto value = child("value")) {
    err << value->trace(context);
  }
  return err;
}
