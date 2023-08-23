#include "NodeTypes.hpp"

#include "CNodeClass.hpp"
#include "CNodeType.hpp"

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

//------------------------------------------------------------------------------

uint32_t CNodeCompound::debug_color() const {
  return 0xFF8888;
}

Err CNodeCompound::trace(CInstance* instance) {
  Err err;
  for (auto c : this) err << c->trace(instance);
  return err;
}

//------------------------------------------------------------------------------
