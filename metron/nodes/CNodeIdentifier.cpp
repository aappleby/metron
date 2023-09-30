#include "CNodeIdentifier.hpp"

#include "metron/Emitter.hpp"
#include "metron/CInstance.hpp"
#include "metron/nodes/CNodeCompound.hpp"

//==============================================================================

uint32_t CNodeIdentifier::debug_color() const { return 0x80FF80; }

//------------------------------------------------------------------------------

std::string_view CNodeIdentifier::get_name() const {
  return get_text();
}

//------------------------------------------------------------------------------

Err CNodeIdentifier::trace(CInstance* inst, call_stack& stack) {
  auto scope = ancestor<CNodeCompound>();

  if (auto inst_field = inst->resolve(this)) {
    return inst_field->log_action(this, ACT_READ, stack);
  }
  return Err();
}

//==============================================================================
