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

Err CNodeBinaryExp::trace(IContext* context) {
  Err err;

  err << child("lhs")->trace(context);
  err << child("rhs")->trace(context);
  return Err();
}

//------------------------------------------------------------------------------

Err CNodePrefixExp::trace(IContext* context) {
  Err err;

  auto rhs = child("rhs");
  auto op  = child("prefix")->get_text();
  err << rhs->trace(context);

  if (op == "++" || op == "--") {
    auto inst = context->resolve(rhs);
    err << inst->log_action(this, ACT_READ);
    err << inst->log_action(this, ACT_WRITE);
  }

  return err;
}

//------------------------------------------------------------------------------

Err CNodeSuffixExp::trace(IContext* context) {
  Err err;

  auto lhs = child("lhs");
  auto op  = child("suffix")->get_text();
  err << lhs->trace(context);

  if (op == "++" || op == "--") {
    auto inst = context->resolve(lhs);
    err << inst->log_action(this, ACT_READ);
    err << inst->log_action(this, ACT_WRITE);
  }

  return err;
}

//------------------------------------------------------------------------------

Err CNodeAssignExp::trace(IContext* context) {
  //Err err;
  //err <<
  //return child("lhs")->trace(context, action);
  dump_tree();
  assert(false);
  return Err();
}

//------------------------------------------------------------------------------

std::string_view CNodeIdentifierExp::get_name() const {
  return get_text();
}

Err CNodeIdentifierExp::trace(IContext* context) {
  Err err;
  if (auto field = context->resolve(this)) {
    err << field->log_action(this, ACT_READ);
  }
  return err;
}

//------------------------------------------------------------------------------

Err CNodeConstant::emit(Cursor& cursor) {
  return cursor.emit_raw(this);
}

Err CNodeConstant::trace(IContext* context) {
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodeOperator::debug_color() const {
  return COL_SKY;
}

Err CNodeOperator::emit(Cursor& cursor) {
  return cursor.emit_default(this);
}

Err CNodeOperator::trace(IContext* context) {
  return Err();
}

//------------------------------------------------------------------------------
