#include "CNodeKeyword.hpp"

#include "metron/Cursor.hpp"

//==============================================================================

uint32_t CNodeKeyword::debug_color() const { return 0xFFFF88; }

std::string_view CNodeKeyword::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeKeyword::emit(Cursor& cursor) {
  auto text = get_text();

  if (text == "static" || text == "const" || text == "break") {
    return cursor.comment_out(this);
  }

  if (text == "nullptr") {
    return cursor.emit_replacement(this, "\"\"");
  }

  if (text == "if" || text == "else" || text == "default" || text == "for" || "enum") {
    return cursor.emit_raw(this);
  }

  NODE_ERR("FIXME");
  return Err();
}

Err CNodeKeyword::trace(CInstance* inst, call_stack& stack) {
  return Err();
}

//==============================================================================
