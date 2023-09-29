#include "CNodeUsing.hpp"

//==============================================================================

uint32_t CNodeUsing::debug_color() const {
  return 0x00DFFF;
}

std::string_view CNodeUsing::get_name() const {
  return child("name")->get_text();
}

CHECK_RETURN Err CNodeUsing::emit(Cursor& cursor) {
  Err err;

  err << cursor.skip_over(this);

  err << cursor.emit_print("import ");
  err << cursor.emit_splice(child("name"));
  err << cursor.emit_print("::*;");

  return err;
}

CHECK_RETURN Err CNodeUsing::trace(CInstance* inst, call_stack& stack) {
  return Err();
};

//==============================================================================
