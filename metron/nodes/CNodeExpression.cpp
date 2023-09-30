#include "CNodeExpression.hpp"

#include "metrolib/core/Log.h"
#include "metron/CInstance.hpp"
#include "metron/Cursor.hpp"

#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeList.hpp"

//==============================================================================

uint32_t CNodeExpression::debug_color() const {
  return COL_AQUA;
}

Err CNodeExpression::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  err << cursor.emit_default(this);
  return err << cursor.check_done(this);
}

//==============================================================================
//==============================================================================




















//==============================================================================
//==============================================================================

Err CNodeBinaryExp::trace(CInstance* inst, call_stack& stack) {
  Err err;
  err << child("lhs")->trace(inst, stack);
  err << child("rhs")->trace(inst, stack);
  return err;
}

//==============================================================================
//==============================================================================




















//==============================================================================
//==============================================================================

Err CNodePrefixExp::trace(CInstance* inst, call_stack& stack) {
  Err err;

  auto rhs = child("rhs");
  err << rhs->trace(inst, stack);

  auto op  = child("prefix")->get_text();
  if (op == "++" || op == "--") {
    auto inst_rhs = inst->resolve(rhs);
    if (inst_rhs) {
      err << inst_rhs->log_action(this, ACT_READ, stack);
      err << inst_rhs->log_action(this, ACT_WRITE, stack);
    }
  }

  return err;
}

//----------------------------------------

Err CNodePrefixExp::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_op    = child("prefix");
  auto op = node_op->get_text();

  if (op != "++" && op != "--") {
    return cursor.emit_default(this);
  }

  auto node_class = ancestor<CNodeClass>();
  auto node_func  = ancestor<CNodeFunction>();
  auto node_rhs   = child("rhs");
  auto node_field = node_class->get_field(node_rhs);

  err << cursor.skip_over(this);
  err << cursor.emit_splice(node_rhs);
  if (node_func->method_type == MT_TICK && node_field) {
    err << cursor.emit_print(" <= ");
  }
  else {
    err << cursor.emit_print(" = ");
  }
  err << cursor.emit_splice(node_rhs);
  if (op == "++") {
    err << cursor.emit_print(" + 1");
  }
  else {
    err << cursor.emit_print(" - 1");
  }

  return err << cursor.check_done(this);
}

//==============================================================================
//==============================================================================




















//==============================================================================
//==============================================================================

Err CNodeSuffixExp::trace(CInstance* inst, call_stack& stack) {
  Err err;

  auto lhs    = child("lhs");
  auto suffix = child("suffix");

  err << lhs->trace(inst, stack);

  if (auto array_suffix = suffix->as<CNodeList>()) {
    err << suffix->trace(inst, stack);
    return err;
  }

  auto op  = child("suffix")->get_text();
  if (op == "++" || op == "--") {
    auto inst_lhs = inst->resolve(lhs);
    if (inst_lhs) {
      err << inst_lhs->log_action(this, ACT_READ, stack);
      err << inst_lhs->log_action(this, ACT_WRITE, stack);
    }
  }

  return err;
}

//----------------------------------------

Err CNodeSuffixExp::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_class = ancestor<CNodeClass>();
  auto node_func  = ancestor<CNodeFunction>();
  auto node_op    = child("suffix");
  auto node_lhs   = child("lhs");
  auto node_field = node_class->get_field(node_lhs);

  auto op = node_op->get_text();

  if (op == "++" || op == "--") {
    err << cursor.skip_over(this);
    err << cursor.emit_splice(node_lhs);
    if (node_func->method_type == MT_TICK && node_field) {
      err << cursor.emit_print(" <= ");
    }
    else {
      err << cursor.emit_print(" = ");
    }
    err << cursor.emit_splice(node_lhs);
    if (op == "++") {
      err << cursor.emit_print(" + 1");
    }
    else {
      err << cursor.emit_print(" - 1");
    }
  }
  else {
    err << cursor.emit_default(this);
  }

  return err << cursor.check_done(this);
}

//==============================================================================
//==============================================================================




















//==============================================================================
//==============================================================================

Err CNodeAssignExp::trace(CInstance* inst, call_stack& stack) {
  NODE_ERR("fixme");
  return Err();
}

//==============================================================================
//==============================================================================




















//==============================================================================
//==============================================================================

std::string_view CNodeIdentifierExp::get_name() const {
  return get_text();
}

//----------------------------------------

Err CNodeIdentifierExp::trace(CInstance* inst, call_stack& stack) {

  if (auto inst_field = inst->resolve(this)) {
    return inst_field->log_action(this, ACT_READ, stack);
  }
  /*
  if (auto inst_arg = call->resolve(this)) {
    return inst_arg->log_action(this, ACT_READ, stack);
  }
  */
  return Err();
}

//==============================================================================
//==============================================================================








































//==============================================================================
//==============================================================================

uint32_t CNodeOperator::debug_color() const {
  return COL_SKY;
}

Err CNodeOperator::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  err << cursor.emit_default(this);
  return err << cursor.check_done(this);
}

Err CNodeOperator::trace(CInstance* inst, call_stack& stack) {
  return Err();
}

//------------------------------------------------------------------------------
