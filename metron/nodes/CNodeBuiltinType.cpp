#include "CNodeBuiltinType.hpp"

#include "metron/Cursor.hpp"
#include "metron/Emitter.hpp"

//==============================================================================

std::string_view CNodeBuiltinType::get_name() const { return get_text(); }

//------------------------------------------------------------------------------

Err CNodeBuiltinType::trace(CInstance* inst, call_stack& stack) {
  return Err();
}

//==============================================================================
