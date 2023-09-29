#include "CNodeAccess.hpp"

#include "metron/Cursor.hpp"

//------------------------------------------------------------------------------

uint32_t CNodeAccess::debug_color() const { return COL_VIOLET; }

CHECK_RETURN Err CNodeAccess::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  err << cursor.comment_out(this);
  return err << cursor.check_done(this);
}

void CNodeAccess::dump() const {
  auto text = get_text();
  LOG_B("CNodeAccess \"%.*s\"\n", text.size(), text.data());
}

//------------------------------------------------------------------------------
