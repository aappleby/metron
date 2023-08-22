#include "NodeTypes.hpp"

#include "CNodeClass.hpp"

using namespace matcheroni;
using namespace parseroni;

//------------------------------------------------------------------------------

Err CNodeTemplate::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_template = child("template");
  auto node_params   = child("params");
  auto node_class    = child("class");

  err << cursor.skip_over(node_template);
  err << cursor.skip_gap_after(node_template);

  err << cursor.skip_over(node_params);
  err << cursor.skip_gap_after(node_params);

  err << cursor.emit(node_class);

  /*
  dump_tree();
  LOG_R("FIXME\n");
  exit(-1);
  */

  // Template params have to go _inside_ the class definition in Verilog, so
  // we skip them here.
  /*
  auto class_node = child("template_class");
  err << cursor.skip_span(cursor.text_cursor, class_node->text_begin());
  err << cursor.emit(class_node);
  err << cursor.skip_span(cursor.text_cursor, text_end());
  */
  //err << emit_rest(cursor);

  //cursor.current_mod.pop();
  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

Err CNodeAccess::emit(Cursor& cursor) {
  Err err;
  err << cursor.comment_out(this);
  return err;
}

//------------------------------------------------------------------------------

Err CNodePreproc::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  auto v = get_text();

  if (v.starts_with("#include")) {
    err << cursor.emit_replacements(this, "#include", "`include", ".h", ".sv");
  }
  else if (v.starts_with("#ifndef")) {
    err << cursor.emit_replacements(this, "#ifndef", "`ifndef");
  }
  else if (v.starts_with("#define")) {
    err << cursor.emit_replacements(this, "#define", "`define");
  }
  else if (v.starts_with("#endif")) {
    err << cursor.emit_replacements(this, "#endif", "`endif");
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
  }
  else {
    err << CNode::emit(cursor);
  }

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------
