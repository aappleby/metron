#include "CNodeExpression.hpp"

#include "metrolib/core/Log.h"
#include "CInstance.hpp"

//------------------------------------------------------------------------------

uint32_t CNodeExpression::debug_color() const {
  return COL_AQUA;
}

Err CNodeExpression::emit(Cursor& cursor) {
  return cursor.emit_default(this);
}

bool CNodeExpression::is_integer_constant() {
  //if (child_count() != 1) return false;
  /*
  auto node_unit = child("unit");
  if (!node_unit || node_unit->child_count() != 1) return false;
  auto node_constant = node_unit->child("constant");
  if (!node_constant) return false;
  auto node_int = node_constant->child("int");
  if (!node_int) return false;
  */
  return false;
}

//------------------------------------------------------------------------------

Err CNodeBinaryExp::trace(CInstance* instance, TraceAction action) {
  Err err;

  err << child("lhs")->trace(instance, action);
  err << child("rhs")->trace(instance, action);
  return Err();
}

//------------------------------------------------------------------------------

Err CNodePrefixExp::trace(CInstance* instance, TraceAction action) {
  return child("rhs")->trace(instance, action);
}

//------------------------------------------------------------------------------

Err CNodeSuffixExp::trace(CInstance* instance, TraceAction action) {
  return child("lhs")->trace(instance, action);
}

//------------------------------------------------------------------------------

Err CNodeAssignExp::trace(CInstance* instance, TraceAction action) {
  //Err err;
  //err <<
  //return child("lhs")->trace(instance, action);
  dump_tree();
  assert(false);
  return Err();
}

//------------------------------------------------------------------------------

Err CNodeIdentifierExp::trace(CInstance* instance, TraceAction action) {
  auto field = instance->resolve(get_text());
  return field->log_action(this, action);
}

//------------------------------------------------------------------------------

Err CNodeConstant::emit(Cursor& cursor) {
  return cursor.emit_raw(this);
}

Err CNodeConstant::trace(CInstance* instance, TraceAction action) {
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodeOperator::debug_color() const {
  return COL_SKY;
}

Err CNodeOperator::emit(Cursor& cursor) {
  return cursor.emit_default(this);
}

Err CNodeOperator::trace(CInstance* instance, TraceAction action) {
  return Err();
}

//------------------------------------------------------------------------------
