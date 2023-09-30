#include "metron/nodes/CNodeType.hpp"

#include "metron/Emitter.hpp"
#include "metrolib/core/Log.h"

//==============================================================================

uint32_t CNodeType::debug_color() const {
  return COL_VIOLET;
}

//------------------------------------------------------------------------------

Err CNodeType::emit(Cursor& cursor) {
  return Emitter(cursor).emit(this);
}

//==============================================================================
