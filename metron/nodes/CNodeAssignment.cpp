#include "CNodeAssignment.hpp"

#include "metron/Cursor.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeLValue.hpp"

//==============================================================================
// For "x[y] = z;", we don't want to allow two writes to x as that would imply
// two write ports on a block mem. So we log a read on x before the write if
// there's an array suffix.

Err CNodeAssignment::trace(CInstance* inst, call_stack& stack) {
  Err err;

  auto rhs    = child("rhs");
  auto op     = child("op");
  auto lhs    = child("lhs")->req<CNodeLValue>();
  auto suffix = lhs->child("suffix");

  err << rhs->trace(inst, stack);

  if (suffix) {
    err << suffix->trace(inst, stack);
  }

  auto inst_lhs = inst->resolve(lhs->child("name"));
  if (!inst_lhs) return err;

  if (op->get_text() != "=" || suffix) {
    err << inst_lhs->log_action(this, ACT_READ, stack);
  }

  err << inst_lhs->log_action(this, ACT_WRITE, stack);

  return err;
}

//----------------------------------------

// Change '=' to '<=' if lhs is a field and we're inside a sequential block.
// Change "a += b" to "a = a + b", etc.

Err CNodeAssignment::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_class = ancestor<CNodeClass>();
  auto node_func  = ancestor<CNodeFunction>();
  auto node_lhs   = child("lhs")->req<CNodeLValue>();
  auto node_op    = child("op");
  auto node_rhs   = child("rhs");
  auto node_field = node_class->get_field(node_lhs);

  err << cursor.emit(node_lhs);
  err << cursor.emit_gap();

  // If we're in a tick, emit < to turn = into <=
  if (node_func->method_type == MT_TICK && node_field) {
    err << cursor.emit_print("<");
  }

  if (node_op->get_text() == "=") {
    err << cursor.emit(node_op);
    err << cursor.emit_gap();
  }
  else {
    auto lhs_text = node_lhs->get_text();

    err << cursor.skip_over(node_op);
    err << cursor.emit_print("=");
    err << cursor.emit_gap();
    err << cursor.emit_print("%.*s %c ", lhs_text.size(), lhs_text.data(), node_op->get_text()[0]);
  }

  err << cursor.emit(node_rhs);

  return err << cursor.check_done(this);
}

//==============================================================================
