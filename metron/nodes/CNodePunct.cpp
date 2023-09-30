#include "CNodePunct.hpp"

#include "metron/Cursor.hpp"

//==============================================================================

uint32_t CNodePunct::debug_color() const { return 0x88FF88; }

std::string_view CNodePunct::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodePunct::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  err << cursor.emit_default(this);
  return err << cursor.check_done(this);
}

Err CNodePunct::trace(CInstance* inst, call_stack& stack) {
  return Err();
}

//==============================================================================
