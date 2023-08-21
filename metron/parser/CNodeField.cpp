#include "CNodeField.hpp"

#include "NodeTypes.hpp"

#include "CNodeClass.hpp"
#include "CNodeStruct.hpp"
#include "CInstance.hpp"

#include "metrolib/core/Log.h"

//------------------------------------------------------------------------------

void CNodeField::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);
}

//------------------------------------------------------------------------------

std::string_view CNodeField::get_name() const {
  return child("decl_name")->get_name();
}

//------------------------------------------------------------------------------

std::string_view CNodeField::get_type_name() const {
  auto decl_type = child("decl_type");
  return decl_type->child_head->get_text();

  /*
  auto decl_type = child("decl_type");
  if (auto name = decl_type->child("type_name")) {
    return name->get_text();
  }

  if (auto builtin = decl_type->child("builtin_name")) {
    return builtin->get_text();
  }

  assert(false);
  return "<could not get type of field>";
  */
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

bool CNodeField::is_component() const {
  return _type_class != nullptr;
}

bool CNodeField::is_struct() const {
  return _type_struct != nullptr;
}

bool CNodeField::is_array() const {
  return child("decl_array") != nullptr;
}

bool CNodeField::is_enum() const {
  return _enum;
}

bool CNodeField::is_port() const {
  return is_public();
}

bool CNodeField::is_param() const {
  return _static && _const;
}

bool CNodeField::is_public() const {
  return _public;
}

bool CNodeField::is_private() const {
  return !_public;
}

//------------------------------------------------------------------------------

bool CNodeField::is_input() const {
  return _state == CTX_INPUT;
}

bool CNodeField::is_register() const {
  return _state == CTX_REGISTER || _state == CTX_MAYBE;
}

bool CNodeField::is_signal() const {
  return _state == CTX_OUTPUT || _state == CTX_SIGNAL;
}

bool CNodeField::is_dead() const {
  return _state == CTX_NONE;
}

//------------------------------------------------------------------------------

void CNodeField::dump() {
  auto name = get_name();
  LOG_A("Field %.*s : ", name.size(), name.data());

  if (_static)    LOG_A("static ");
  if (_const)     LOG_A("const ");
  if (_public)    LOG_A("public ");
  if (is_array()) LOG_A("array ");
  if (_enum)      LOG_A("enum ");

  if (_parent_class) {
    auto name = _parent_class->get_name();
    LOG_A("parent %.*s ", int(name.size()), name.data());
  }

  if (_parent_struct) {
    auto name = _parent_struct->get_name();
    LOG_A("parent %.*s ", int(name.size()), name.data());
  }

  if (_type_class) {
    auto name = _type_class->get_name();
    LOG_A("type %.*s ", int(name.size()), name.data());
  }

  if (_type_struct) {
    auto name = _type_struct->get_name();
    LOG_A("type %.*s ", int(name.size()), name.data());
  }

  LOG_A("\n");
}
