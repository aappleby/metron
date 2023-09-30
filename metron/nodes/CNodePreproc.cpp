#include "CNodePreproc.hpp"

#include "metron/Cursor.hpp"

//==============================================================================

uint32_t CNodePreproc::debug_color() const { return 0x00BBBB; }

//------------------------------------------------------------------------------

std::string_view CNodePreproc::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

//------------------------------------------------------------------------------

Err CNodePreproc::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  if (tag_is("preproc_define")) {
    err << cursor.emit_default(this);
    auto name = child("name")->get_textstr();
    cursor.preproc_vars.insert(name);
  }

  else if (tag_is("preproc_include")) {
    err << cursor.emit_replacements(this, "#include", "`include", ".h", ".sv");
  }

  else {
    err << cursor.emit_default(this);
  }

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

Err CNodePreproc::trace(CInstance* inst, call_stack& stack) {
  NODE_ERR("FIXME");
  return Err();
}

//==============================================================================
