#include "CNodeTranslationUnit.hpp"

#include "metron/Emitter.hpp"

//==============================================================================

uint32_t CNodeTranslationUnit::debug_color() const { return 0xFFFF00; }

std::string_view CNodeTranslationUnit::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeTranslationUnit::trace(CInstance* inst, call_stack& stack) {
  NODE_ERR("FIXME");
  return Err();
}

//==============================================================================
