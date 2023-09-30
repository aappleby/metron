#include "CNodeAccess.hpp"

#include "metron/Cursor.hpp"
#include "metron/Emitter.hpp"

//==============================================================================

uint32_t CNodeAccess::debug_color() const { return COL_VIOLET; }

//------------------------------------------------------------------------------

void CNodeAccess::dump() const {
  auto text = get_text();
  LOG_B("CNodeAccess \"%.*s\"\n", text.size(), text.data());
}

//==============================================================================
