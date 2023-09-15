#include "CNodeExpression.hpp"

#include "metrolib/core/Log.h"
#include "CInstance.hpp"

//------------------------------------------------------------------------------

uint32_t CNodeExpression::debug_color() const {
  return COL_AQUA;
}

Err CNodeExpression::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  err << cursor.emit_default(this);
  return err << cursor.check_done(this);
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

//----------------------------------------

Err CNodePrefixExp::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto func = ancestor<CNodeFunction>();

  auto node_op  = child("prefix");
  auto node_rhs = child("rhs");

  auto op = node_op->get_text();

  if (op == "++" || op == "--") {
    err << cursor.skip_over(this);
    err << cursor.emit_splice(node_rhs);
    if (func->method_type == MT_TICK) {
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
  }
  else {
    err << cursor.emit_default(this);
  }

  return err << cursor.check_done(this);
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

//----------------------------------------

Err CNodeSuffixExp::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto func = ancestor<CNodeFunction>();

  auto node_lhs = child("lhs");
  auto node_op  = child("suffix");

  auto op = node_op->get_text();

  if (op == "++" || op == "--") {
    err << cursor.skip_over(this);
    err << cursor.emit_splice(node_lhs);
    if (func->method_type == MT_TICK) {
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

Err CNodeConstant::trace(CCall* call) {
  return Err();
}

//----------------------------------------


Err CNodeConstant::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  std::string body = get_textstr();

  // FIXME what was this for?
  //int size_cast = override_size.top();
  int size_cast = 0;

  // Count how many 's are in the number
  int spacer_count = 0;
  int prefix_count = 0;

  for (auto& c : body)
    if (c == '\'') {
      c = '_';
      spacer_count++;
    }

  if (body.starts_with("0x")) {
    prefix_count = 2;
    if (!size_cast) size_cast = ((int)body.size() - 2 - spacer_count) * 4;
    err << cursor.emit_print("%d'h", size_cast);
  } else if (body.starts_with("0b")) {
    prefix_count = 2;
    if (!size_cast) size_cast = (int)body.size() - 2 - spacer_count;
    err << cursor.emit_print("%d'b", size_cast);
  } else {
    if (size_cast) {
      err << cursor.emit_print("%d'd", size_cast);
    }
  }

  err << cursor.emit_print("%s", body.c_str() + prefix_count);

  cursor.gap_emitted = false;
  cursor.tok_cursor = tok_end();

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

uint32_t CNodeOperator::debug_color() const {
  return COL_SKY;
}

Err CNodeOperator::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  err << cursor.emit_default(this);
  return err << cursor.check_done(this);
}

Err CNodeOperator::trace(CCall* call) {
  return Err();
}

//------------------------------------------------------------------------------
