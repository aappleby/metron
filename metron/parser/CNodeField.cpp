#include "CNodeField.hpp"

#include "NodeTypes.hpp"

#include "CNodeClass.hpp"
#include "CNodeStruct.hpp"
#include "CInstance.hpp"

#include "metrolib/core/Log.h"

//------------------------------------------------------------------------------

uint32_t CNodeField::debug_color() const {
  return COL_PINK;
}

std::string_view CNodeField::get_name() const {
  return child("name")->get_name();
}

//------------------------------------------------------------------------------

void CNodeField::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);
}

//------------------------------------------------------------------------------

std::string_view CNodeField::get_type_name() const {
  auto decl_type = child("type");
  return decl_type->child_head->get_text();

  /*
  auto decl_type = child("type");
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

bool CNodeField::is_component() const {
  return _type_class != nullptr;
}

bool CNodeField::is_struct() const {
  return _type_struct != nullptr;
}

bool CNodeField::is_array() const {
  return child("array") != nullptr;
}

bool CNodeField::is_const_char() const {
  auto node_static = child("static");
  auto node_const  = child("const");
  auto node_type   = child("type");

  if (node_static && node_const) {
    auto builtin = node_type->child("builtin_name");
    auto star    = node_type->child("star");
    if (builtin && star && builtin->get_text() == "char") {
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------

#if 0
//------------------------------------------------------------------------------
// Emit field declarations. For components, also emit glue declarations and
// append the glue parameter list to the field.

// + field_declaration (259) =
// |--# type: type_identifier (444) = "example_data_memory"
// |--# declarator: field_identifier (440) = "data_memory"
// |--# lit (39) = ";"

CHECK_RETURN Err MtCursor::emit_sym_field_declaration(MnNode n) {
  Err err = check_at(n);
  assert(n.sym == sym_field_declaration);

  //----------
  // Actual fields

  auto field = current_mod.top()->get_field(n.name4());
  assert(field);

  if (field->is_component()) {
    // Component
    err << emit_component(n);
    err << emit_submod_binding_fields(n);
  }
  else if (field->is_port()) {
    // Ports don't go in the class body.
    err << skip_over(n);
  }
  else if (field->is_dead()) {
    err << comment_out(n);
  }
  else {
    err << emit_children(n);
  }

  return err << check_done(n);
}

/*
[000.013] ========== tree dump begin
[000.013]  ┳━ CNodeField field =
[000.013]  ┣━━┳━ CNodeType decl_type =
[000.013]  ┃  ┗━━━━ CNode builtin_name = "int"
[000.013]  ┗━━━━ CNodeIdentifier decl_name = "x"
[000.013] ========== tree dump end
*/

if (n.is_const()) {
  err << emit_ws_to(n);
  err << emit_print("localparam ");
  err << emit_children(n);
  return err << check_done(n);
}

#endif


Err CNodeField::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_static = child("static");
  auto node_const  = child("const");
  auto node_type   = child("type");
  auto node_name   = child("name");
  auto node_eq     = child("eq");
  auto node_value  = child("value");

  if (is_const_char()) {
    err << cursor.skip_over(node_static);
    err << cursor.skip_gap_after(node_static);
    err << cursor.skip_over(node_const);
    err << cursor.skip_gap_after(node_const);
    err << cursor.skip_over(node_type);
    err << cursor.skip_gap_after(node_type);
    err << cursor.emit_print("localparam string ");
    err << cursor.emit(node_name);
    err << cursor.emit_gap_after(node_name);
    err << cursor.emit(node_eq);
    err << cursor.emit_gap_after(node_eq);
    err << cursor.emit(node_value);
    err << cursor.emit_gap_after(node_value);
    return err << cursor.check_done(this);
  }

  if (node_static && node_const) {
    // Localparam
    err << cursor.emit_print("localparam ");
    err << cursor.comment_out(node_static);
    err << cursor.emit_gap_after(node_static);
    err << cursor.comment_out(node_const);
    err << cursor.emit_gap_after(node_const);
    err << cursor.emit(node_type);
    err << cursor.emit_gap_after(node_type);
    err << cursor.emit(node_eq);
    err << cursor.emit_gap_after(node_eq);
    err << cursor.emit(node_value);
    err << cursor.emit_gap_after(node_value);
    return err << cursor.check_done(this);
  }

  auto node_builtin = node_type->child("builtin_name");
  auto node_targs    = node_type->child("template_args");

  if (node_builtin && node_targs) {
    err << cursor.comment_out(node_static);
    err << cursor.emit_gap_after(node_static);
    err << cursor.comment_out(node_const);
    err << cursor.emit_gap_after(node_const);
    err << cursor.emit(node_type);
    err << cursor.emit_gap_after(node_type);
    err << cursor.emit(node_name);
    err << cursor.emit_gap_after(node_name);
    err << cursor.emit(node_eq);
    err << cursor.emit_gap_after(node_eq);
    err << cursor.emit(node_value);
    err << cursor.emit_gap_after(node_value);

    return err;
  }

  if (is_struct()) {
  }


  if (is_component()) {
    return err << CNode::emit(cursor);
  }

  //err << cursor.emit_default(this);
  err << CNode::emit(cursor);


  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeField::trace(CCall* call) {
  return child("value")->trace(call);
}

//------------------------------------------------------------------------------

/*
  TraceState _state = CTX_PENDING;

  CNodeClass*  _parent_class;
  CNodeStruct* _parent_struct;

  CNodeClass*  _type_class;
  CNodeStruct* _type_struct;

  bool _static = false;
  bool _const = false;
  bool _public = false;
  bool _enum = false;
*/

void CNodeField::dump() {
  //dump_tree();

  auto name = get_name();
  LOG_A("Field %.*s : ", name.size(), name.data());

  if (child("static")) LOG_A("static ");
  if (child("const"))  LOG_A("const ");
  if (_public)    LOG_A("public ");
  if (is_array()) LOG_A("array ");
  if (_enum)      LOG_A("enum ");

  if (_parent_class) {
    auto name = _parent_class->get_name();
    LOG_A("parent class %.*s ", int(name.size()), name.data());
  }

  if (_parent_struct) {
    auto name = _parent_struct->get_name();
    LOG_A("parent struct %.*s ", int(name.size()), name.data());
  }

  if (_type_class) {
    auto name = _type_class->get_name();
    LOG_A("type class %.*s ", int(name.size()), name.data());
  }

  if (_type_struct) {
    auto name = _type_struct->get_name();
    LOG_A("type struct %.*s ", int(name.size()), name.data());
  }

  LOG_A("%s", to_string(field_type));

  LOG_A("\n");
}
