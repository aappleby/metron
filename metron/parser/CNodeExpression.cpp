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

Err CNodeBinaryExp::trace(CInstance* inst, call_stack& stack) {
  Err err;
  err << child("lhs")->trace(inst, stack);
  err << child("rhs")->trace(inst, stack);
  return err;
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

Err CNodeAssignExp::trace(CInstance* inst, call_stack& stack) {
  NODE_ERR("fixme");
  return Err();
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

Err CNodeConstant::trace(CInstance* inst, call_stack& stack) {
  return Err();
}

//----------------------------------------


Err CNodeConstant::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  std::string body = get_textstr();

  // FIXME what was this for?
  // This was for forcing enum constants to the size of the enum type
  int size_cast = cursor.override_size.top();
  //int size_cast = 0;

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

Err CNodeOperator::trace(CInstance* inst, call_stack& stack) {
  return Err();
}

//------------------------------------------------------------------------------
