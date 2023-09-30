#include "CNodeExpression.hpp"

#include "metrolib/core/Log.h"
#include "metron/CInstance.hpp"
#include "metron/Emitter.hpp"

#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeList.hpp"

//==============================================================================
//==============================================================================

uint32_t CNodeExpression::debug_color() const {
  return COL_AQUA;
}

Err CNodeExpression::emit(Cursor& cursor) {
  return Emitter(cursor).emit(this);
}

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
  return Emitter(cursor).emit(this);
}

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
  return Emitter(cursor).emit(this);
}

//==============================================================================
//==============================================================================

Err CNodeAssignExp::trace(CInstance* inst, call_stack& stack) {
  NODE_ERR("fixme");
  return Err();
}

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
  return Err();
}

//==============================================================================
//==============================================================================

uint32_t CNodeOperator::debug_color() const {
  return COL_SKY;
}

Err CNodeOperator::emit(Cursor& cursor) {
  return Emitter(cursor).emit(this);
}

Err CNodeOperator::trace(CInstance* inst, call_stack& stack) {
  return Err();
}

//==============================================================================
//==============================================================================
