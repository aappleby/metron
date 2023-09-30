#include "CNodeBuiltinType.hpp"

#include "metron/Cursor.hpp"
#include "metron/Emitter.hpp"

//==============================================================================

std::string_view CNodeBuiltinType::get_name() const { return get_text(); }

//------------------------------------------------------------------------------

Err CNodeBuiltinType::trace(CInstance* inst, call_stack& stack) {
  return Err();
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeBuiltinType::emit(Cursor& cursor) {
  return Emitter(cursor).emit(this);
}

//==============================================================================
