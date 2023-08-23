#include "CNodeExpression.hpp"

#include "metrolib/core/Log.h"

//------------------------------------------------------------------------------

uint32_t CNodeUnit::debug_color() const {
  return COL_ORANGE;
}

Err CNodeUnit::emit(Cursor& cursor) {
  return cursor.emit_default(this);
}

Err CNodeUnit::trace(CInstance* instance) {
  return trace_children(instance);
}

//------------------------------------------------------------------------------

uint32_t CNodeExpression::debug_color() const {
  return COL_YELLOW;
}

Err CNodeExpression::emit(Cursor& cursor) {
  return cursor.emit_default(child("unit"));
}

Err CNodeExpression::trace(CInstance* instance) {
  return trace_children(instance);
}

bool CNodeExpression::is_integer_constant() {
  if (child_count() != 1) return false;
  auto node_unit = child("unit");
  if (!node_unit || node_unit->child_count() != 1) return false;
  auto node_constant = node_unit->child("constant");
  if (!node_constant) return false;
  auto node_int = node_constant->child("int");
  if (!node_int) return false;
  return true;
}

//------------------------------------------------------------------------------
