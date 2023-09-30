#include "CNodeUsing.hpp"

#include "metron/Emitter.hpp"

//==============================================================================

uint32_t CNodeUsing::debug_color() const {
  return 0x00DFFF;
}

std::string_view CNodeUsing::get_name() const {
  return child("name")->get_text();
}

CHECK_RETURN Err CNodeUsing::trace(CInstance* inst, call_stack& stack) {
  return Err();
};

//==============================================================================
