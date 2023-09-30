#include "CNodeIdentifier.hpp"

#include "metron/Cursor.hpp"
#include "metron/CInstance.hpp"
#include "metron/nodes/CNodeCompound.hpp"

//==============================================================================

uint32_t CNodeIdentifier::debug_color() const { return 0x80FF80; }

//------------------------------------------------------------------------------

std::string_view CNodeIdentifier::get_name() const {
  return get_text();
}

//------------------------------------------------------------------------------

Err CNodeIdentifier::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  auto text = get_textstr();

  auto& id_map = cursor.id_map.top();
  auto found = id_map.find(text);

  if (found != id_map.end()) {
    auto replacement = (*found).second;
    err << cursor.emit_replacement(this, "%s", replacement.c_str());
  }
  else if (cursor.preproc_vars.contains(text)) {
    err << cursor.emit_print("`");
    err << cursor.emit_default(this);
  }
  else {
    err << cursor.emit_default(this);
  }

  //err << emit_span(n->tok_begin(), n->tok_end());

  return err << cursor.check_done(this);

  //
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
