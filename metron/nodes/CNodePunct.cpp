#include "CNodePunct.hpp"

#include "metron/Emitter.hpp"

//==============================================================================

uint32_t CNodePunct::debug_color() const { return 0x88FF88; }

std::string_view CNodePunct::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodePunct::emit(Cursor& cursor) {
  return Emitter(cursor).emit(this);
}

Err CNodePunct::trace(CInstance* inst, call_stack& stack) {
  return Err();
}

//==============================================================================
