#include "CNodeCall.hpp"

#include "metron/Cursor.hpp"
#include "metron/Emitter.hpp"

#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeConstant.hpp"
#include "metron/nodes/CNodeFieldExpression.hpp"

extern bool deep_trace;

//------------------------------------------------------------------------------

void CNodeCall::init(const char* match_tag, SpanType span, uint64_t flags) {
  node_name  = child("func_name")->req<CNode>();
  node_targs = child("func_targs")->opt<CNodeList>();
  node_args  = child("func_args")->req<CNodeList>();
}

//------------------------------------------------------------------------------

std::string_view CNodeCall::get_name() const {
  return child("func_name")->get_text();
}

//------------------------------------------------------------------------------

uint32_t CNodeCall::debug_color() const {
  return COL_SKY;
}

//------------------------------------------------------------------------------
// FIXME not dealing with nested submod paths right now

Err CNodeCall::trace(CInstance* inst, call_stack& stack) {
  Err err;

  auto src_class = ancestor<CNodeClass>();

  err << node_args->trace(inst, stack);

  auto dst_name = node_name;

  if (auto field_exp = dst_name->as<CNodeFieldExpression>()) {
    auto submod_name = field_exp->child("field_path")->get_textstr();
    auto submod_field = src_class->get_field(submod_name);
    auto func_inst = inst->resolve(field_exp)->as<CInstFunc>();
    auto node_func = func_inst->node_func;

    stack.push_back(func_inst->node_func);
    for (auto child : func_inst->params) {
      err << child->log_action(this, ACT_WRITE, stack);
    }

    if (deep_trace) {
      err << node_func->trace(func_inst, stack);
    }


    if (func_inst->inst_return) {
      err << func_inst->inst_return->log_action(this, ACT_READ, stack);
    }
    stack.pop_back();
  }
  else {
    auto func_name = dst_name->get_textstr();
    auto dst_func = src_class->get_function(func_name);

    auto func_inst = inst->resolve(func_name)->as<CInstFunc>();

    if (dst_func && func_inst) {
      stack.push_back(dst_func);
      err << dst_func->trace(func_inst, stack);
      stack.pop_back();
    }
  }

  return err;
}

//------------------------------------------------------------------------------

bool CNodeCall::is_bit_extract() {
  if (auto func_id = node_name->as<CNodeIdentifier>()) {
    auto func_name = func_id->get_textstr();

    auto args = node_args->items.size();

    if (func_name.size() == 2) {
      if (func_name == "bx") {
        return true;
      }
      else if (func_name[0] == 'b' && isdigit(func_name[1])) {
        return true;
      }
    }
    else if (func_name.size() == 3) {
      if (func_name[0] == 'b' && isdigit(func_name[1]) && isdigit(func_name[2])) {
        return true;
      }
    }
  }
  return false;
}

//------------------------------------------------------------------------------
