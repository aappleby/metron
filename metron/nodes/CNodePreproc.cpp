#include "CNodePreproc.hpp"

#include "metron/Emitter.hpp"

//==============================================================================

uint32_t CNodePreproc::debug_color() const { return 0x00BBBB; }

//------------------------------------------------------------------------------

std::string_view CNodePreproc::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

//------------------------------------------------------------------------------

Err CNodePreproc::trace(CInstance* inst, call_stack& stack) {
  NODE_ERR("FIXME");
  return Err();
}

//==============================================================================
