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

Err CNodeCall::trace(CCall* call) {
  Err err;

  //dump_tree();

  auto node_func_name = child("func_name");

  //node_func_name->dump_tree();

  if (auto field_exp = node_func_name->as_a<CNodeFieldExpression>()) {
    LOG_R("Submod call\n");
    // FIXME not dealing with nested submod paths right now
    auto submod_name = field_exp->child("field_path");

    //auto submod_inst = call->inst_class->class_map[submod_name->get_text()];
    auto submod_inst = call->inst_class->inst_map[submod_name->get_text()]->as_a<CInstClass>();

    auto submod_field = call->inst_class->node_class->get_field(submod_name->get_text());

    //submod_field->dump_tree();

    auto submod_class_name = submod_field->child("type")->get_text();

    auto repo = call->inst_class->node_class->repo;

    auto submod_class_node = repo->get_class(submod_class_name);

    //submod_class_node->dump_tree();

    auto func_name = field_exp->child("identifier");

    //func_name->dump_tree();

    auto func_node = submod_class_node->get_function(func_name->get_text());

    //func_node->dump_tree();


    auto dst_call  = new CCall(submod_inst, this, func_node);

    call->inst_call_map[this] = dst_call;

    err << func_node->trace(dst_call);
    return err;

    //auto submod_inst  = call->inst_class->resolve(submod_name)->as_a<CInstField>();

    //LOG_R("submod field %p\n", submod_field);
    //LOG_R("submod inst  %p\n", submod_inst);

    //submod_inst->dump_tree();

    //LOG_R("lksjfdlkj\n");
    //assert(false);

  }
  else {
    LOG_R("Method call\n");
    auto dst_func_name = child("func_name");
    auto dst_func = call->inst_class->node_class->get_function(dst_func_name->get_text());
    auto dst_call  = new CCall(call->inst_class, this, dst_func);

    call->inst_call_map[this] = dst_call;

    err << dst_func->trace(dst_call);
    return err;
  }

  /*
  for (auto arg : child("func_args")) {
    err << arg->trace(call, inst);
  }



  {
    auto node_func = node_class->resolve(node);
    if (node_func) {
      auto name = node_func->get_name();
      LOG_R("Instantiating functon %.*s\n", int(name.size()), name.data());

    }
  }
  */


  //auto dst_func_class = dst_func_node->ancestor<CNodeClass>();

  //assert(dst_func_node);

  /*
  auto dst_inst_call  = new CInstCall(dst_inst_class, dst_func, this);
  src_inst->inst_calls.push_back(dst_inst);

  for (auto a : dst_inst->inst_args) {
    err << a->log_action(this, ACT_WRITE);
  }

  err << dst_func_node->trace(dst_inst);

  if (dst_inst->inst_return) {
    err << dst_inst->inst_return->log_action(this, ACT_READ);
  }
  */
  assert(false);

  return err;
}

//------------------------------------------------------------------------------
