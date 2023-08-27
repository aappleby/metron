#include "CNodeCall.hpp"

#include "CInstance.hpp"

//------------------------------------------------------------------------------

std::string_view CNodeCall::get_name() const {
  return child("func_name")->get_text();
}

uint32_t CNodeCall::debug_color() const {
  return COL_SKY;
}

Err CNodeCall::emit(Cursor& c) {
  assert(false);
  return Err();
}

Err CNodeCall::trace(CInstance* instance, TraceAction action) {
  Err err;
  dump_tree();

  for (auto arg : child("func_args")) {
    arg->trace(instance, ACT_READ);
  }

  CInstFunction* inst_function = dynamic_cast<CInstFunction*>(instance);
  CInstCall* inst_call = inst_function->get_call(this);

  //for (auto arg : inst_call->inst_args) {
  //  arg->trace(ACT_WRITE);
  //}

  err << inst_call->trace(ACT_READ);

  //err << inst_call->inst_return->trace(ACT_READ);

  assert(false);
  return err;
}

//------------------------------------------------------------------------------
