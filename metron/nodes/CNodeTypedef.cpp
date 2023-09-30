#include "CNodeTypedef.hpp"

#include "metron/Emitter.hpp"

//==============================================================================

uint32_t CNodeTypedef::debug_color() const { return 0xFFFF88; }

std::string_view CNodeTypedef::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeTypedef::trace(CInstance* inst, call_stack& stack) {
  NODE_ERR("FIXME");
  return Err();
}

//==============================================================================
