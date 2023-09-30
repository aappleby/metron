#include "CNodeAssignment.hpp"

#include "metron/Cursor.hpp"
#include "metron/Emitter.hpp"
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

Err CNodeAssignment::emit(Cursor& cursor) {
  return Emitter(cursor).emit(this);
}

//==============================================================================
