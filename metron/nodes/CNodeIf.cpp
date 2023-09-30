#include "CNodeIf.hpp"

#include "metron/Emitter.hpp"
#include "metron/nodes/CNodeCall.hpp"
#include "metron/nodes/CNodeExpStatement.hpp"
#include "metron/nodes/CNodeFieldExpression.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeList.hpp"

//==============================================================================

void CNodeIf::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  node_kw_if = child("if")->req<CNodeKeyword>();
  node_cond = child("condition")->req<CNodeList>();
  node_then = child("body_true")->req<CNodeStatement>();
  node_kw_else = child("else")->as<CNodeKeyword>();
  node_else = child("body_false")->as<CNodeStatement>();
}

//----------------------------------------

CHECK_RETURN Err CNodeIf::trace(CInstance* inst, call_stack& stack) {
  Err err;

  auto body_true = child("body_true");

  auto body_false = child("body_false");

  if (auto exp_true = body_true->as<CNodeExpStatement>()) {
    if (auto node_call = exp_true->node_exp->as<CNodeCall>()) {
      auto node_name = node_call->child("func_name");
      if (auto node_field = node_name->as<CNodeFieldExpression>()) {
        return ERR("If branches that contain component calls must use {}.\n");
      }
    }
  }

  if (auto exp_false = body_false->as<CNodeExpStatement>()) {
    if (auto node_call = exp_false->node_exp->as<CNodeCall>()) {
      auto node_name = node_call->child("func_name");
      if (auto node_field = node_name->as<CNodeFieldExpression>()) {
        return ERR("Else branches that contain component calls must use {}.\n");
      }
    }
  }

  err << child("condition")->trace(inst, stack);

  auto root_inst = inst->get_root();

  root_inst->push_state();
  if (body_true) err << body_true->trace(inst, stack);
  root_inst->swap_state();
  if (body_false) err << body_false->trace(inst, stack);
  root_inst->merge_state();

  return err;
}

//----------------------------------------

CHECK_RETURN Err CNodeIf::emit(Cursor& cursor) {
  return Emitter(cursor).emit(this);
}

//==============================================================================
