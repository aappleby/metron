#include "CNodeReturn.hpp"

#include "metron/CInstance.hpp"
#include "metron/Cursor.hpp"
#include "metron/nodes/CNodeFunction.hpp"

//==============================================================================

Err CNodeReturn::trace(CInstance* inst, call_stack& stack) {
  Err err;

  if (auto node_value = child("value")) {
    err << node_value->trace(inst, stack);
  }

  auto inst_return = inst->resolve("@return");
  assert(inst_return);
  err << inst_return->log_action(this, ACT_WRITE, stack);

  return err;
}

//------------------------------------------------------------------------------

Err CNodeReturn::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto func = ancestor<CNodeFunction>();
  auto fname = func->get_namestr();

  auto node_ret  = child("return");
  auto node_val  = child("value");
  auto node_semi = child("semi");

  assert(node_val);

  err << cursor.skip_over(node_ret);
  err << cursor.skip_gap();

  if (func->emit_as_task() || func->emit_as_func()) {
    err << cursor.emit_print("%s = ", fname.c_str());
  }
  else {
    err << cursor.emit_print("%s_ret = ", fname.c_str());
  }

  err << cursor.emit(node_val);
  err << cursor.emit_gap();

  err << cursor.emit(node_semi);

  return err << cursor.check_done(this);
}

//==============================================================================
