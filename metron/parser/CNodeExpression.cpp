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

Err CNodeBinaryExp::trace(CCall* call) {
  Err err;

  err << child("lhs")->trace(call);
  err << child("rhs")->trace(call);
  return Err();
}

//------------------------------------------------------------------------------

Err CNodePrefixExp::trace(CCall* call) {
  Err err;

  auto rhs = child("rhs");
  err << rhs->trace(call);

  auto op  = child("prefix")->get_text();
  if (op == "++" || op == "--") {
    auto inst_rhs = call->inst_class->resolve(rhs);
    if (inst_rhs) {
      err << inst_rhs->log_action(this, ACT_READ);
      err << inst_rhs->log_action(this, ACT_WRITE);
    }
  }

  return err;
}

//------------------------------------------------------------------------------

Err CNodeSuffixExp::trace(CCall* call) {
  Err err;

  auto lhs = child("lhs");
  auto op  = child("suffix")->get_text();
  err << lhs->trace(call);

  if (op == "++" || op == "--") {
    auto inst_lhs = call->inst_class->resolve(lhs);
    if (inst_lhs) {
      err << inst_lhs->log_action(this, ACT_READ);
      err << inst_lhs->log_action(this, ACT_WRITE);
    }
  }

  return err;
}

//------------------------------------------------------------------------------

Err CNodeAssignExp::trace(CCall* call) {
  NODE_ERR("fixme");
  return Err();
}

//------------------------------------------------------------------------------

std::string_view CNodeIdentifierExp::get_name() const {
  return get_text();
}

Err CNodeIdentifierExp::trace(CCall* call) {
  if (auto inst_field = call->inst_class->resolve(this)) {
    return inst_field->log_action(this, ACT_READ);
  }
  /*
  if (auto inst_arg = call->resolve(this)) {
    return inst_arg->log_action(this, ACT_READ);
  }
  */
  return Err();
}

//------------------------------------------------------------------------------

Err CNodeConstant::emit(Cursor& cursor) {
  return cursor.emit_raw(this);
}

Err CNodeConstant::trace(CCall* call) {
  return Err();
}

//------------------------------------------------------------------------------

uint32_t CNodeOperator::debug_color() const {
  return COL_SKY;
}

Err CNodeOperator::emit(Cursor& cursor) {
  return cursor.emit_default(this);
}

Err CNodeOperator::trace(CCall* call) {
  return Err();
}

//------------------------------------------------------------------------------
