#include "NodeTypes.hpp"

using namespace matcheroni;
using namespace parseroni;

//------------------------------------------------------------------------------

Err CNodeTemplate::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  // Template params have to go _inside_ the class definition in Verilog, so
  // we skip them here.
  auto class_node = child("template_class");
  err << cursor.skip_span(cursor.text_cursor, class_node->text_begin());
  err << cursor.emit(class_node);
  err << cursor.skip_span(cursor.text_cursor, text_end());
  //err << emit_rest(cursor);

  //cursor.current_mod.pop();
  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

Err CNodePreproc::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  auto v = get_text();

  if (v.starts_with("#include")) {
    err << cursor.emit_replacements(v, "#include", "`include", ".h", ".sv");
  }
  else if (v.starts_with("#ifndef")) {
    err << cursor.emit_replacements(v, "#ifndef", "`ifndef");
  }
  else if (v.starts_with("#define")) {
    err << cursor.emit_replacements(v, "#define", "`define");
  }
  else if (v.starts_with("#endif")) {
    err << cursor.emit_replacements(v, "#endif", "`endif");
  }
  else {
    return ERR("Don't know how to handle this preproc");
  }

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

Err CNodeDeclaration::emit(Cursor& cursor) {
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
    err << cursor.emit_replacement(this, "{{CNodeDeclaration}}");
  }

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------
