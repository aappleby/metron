#include "CNodeKeyword.hpp"

#include "metron/Emitter.hpp"

//==============================================================================

uint32_t CNodeKeyword::debug_color() const { return 0xFFFF88; }

std::string_view CNodeKeyword::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeKeyword::emit(Cursor& cursor) {
  return Emitter(cursor).emit(this);
}

Err CNodeKeyword::trace(CInstance* inst, call_stack& stack) {
  return Err();
}

//==============================================================================
