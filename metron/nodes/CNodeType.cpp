#include "metron/nodes/CNodeType.hpp"

#include "metrolib/core/Log.h"

//==============================================================================

uint32_t CNodeType::debug_color() const {
  return COL_VIOLET;
}

//------------------------------------------------------------------------------

Err CNodeType::emit(Cursor& cursor) {
  NODE_ERR("Don't know how to handle this type\n");
  return Err();
}

//==============================================================================
