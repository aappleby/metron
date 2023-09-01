#include "CNodeCall.hpp"

#include "CNodeClass.hpp"
#include "CNodeFunction.hpp"
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

Err CNodeCall::trace(IContext* context) {
  Err err;
  dump_tree();
  //context->dump_tree();

  for (auto arg : child("func_args")) {
    err << arg->trace(context);
  }

  auto src_inst  = dynamic_cast<CInstCall*>(context);
  auto src_class = dynamic_cast<CInstClass*>(src_inst->parent);

  auto dst_name  = get_name();
  auto dst_func  = src_class->node_class->get_function(dst_name);
  auto dst_inst  = new CInstCall(src_class, dst_func, this);

  src_inst->inst_calls.push_back(dst_inst);

  for (auto a : dst_inst->inst_args) {
    err << a->log_action(this, ACT_WRITE);
  }

  err << dst_func->trace(dst_inst);

  if (dst_inst->inst_return) {
    err << dst_inst->inst_return->log_action(this, ACT_READ);
  }

  return err;
}

//------------------------------------------------------------------------------
