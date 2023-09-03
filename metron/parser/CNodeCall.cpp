#include "CNodeCall.hpp"

#include "CNodeClass.hpp"
#include "CNodeFunction.hpp"
#include "CInstance.hpp"
#include "NodeTypes.hpp"
#include "CNodeField.hpp"

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

// FIXME not dealing with nested submod paths right now

Err CNodeCall::trace(CCall* call) {
  Err err;
  auto src_inst  = call->inst_class;
  auto src_class = src_inst->node_class;

  err << child("func_args")->trace(call);

  auto dst_name = child("func_name");

  if (auto field_exp = dst_name->as_a<CNodeFieldExpression>()) {
    auto dst_inst  = src_inst->inst_map[field_exp->child("field_path")->get_text()]->as_a<CInstClass>();
    auto dst_class = dst_inst->node_class;
    auto dst_func  = dst_class->get_function(field_exp->child("identifier")->get_text());

    auto dst_call  = new CCall(dst_inst, this, dst_func);
    call->call_map[this] = dst_call;
    err << dst_func->trace(dst_call);
  }
  else {
    auto dst_func = src_class->get_function(dst_name->get_text());
    if (dst_func) {
      auto dst_call = new CCall(src_inst, this, dst_func);
      call->call_map[this] = dst_call;
      err << dst_func->trace(dst_call);
    }
  }

  return err;
}

//------------------------------------------------------------------------------
