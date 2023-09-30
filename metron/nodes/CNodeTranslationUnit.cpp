#include "CNodeTranslationUnit.hpp"

#include "metron/Cursor.hpp"

//==============================================================================

uint32_t CNodeTranslationUnit::debug_color() const { return 0xFFFF00; }

std::string_view CNodeTranslationUnit::get_name() const {
  NODE_ERR("FIXME");
  return "";
}

Err CNodeTranslationUnit::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  for (auto c = child_head; c; c = c->node_next) {
    err << cursor.emit(c);
    if (c->node_next) err << cursor.emit_gap();
  }

  return err << cursor.check_done(this);
}

Err CNodeTranslationUnit::trace(CInstance* inst, call_stack& stack) {
  NODE_ERR("FIXME");
  return Err();
}

//==============================================================================
