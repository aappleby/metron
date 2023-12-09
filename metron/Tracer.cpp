#include "Tracer.hpp"

#include "metron/CInstance.hpp"
#include "metron/CSourceRepo.hpp"
#include "metron/Cursor.hpp"
#include "metron/nodes/CNodeAccess.hpp"
#include "metron/nodes/CNodeAssignment.hpp"
#include "metron/nodes/CNodeType.hpp"
#include "metron/nodes/CNodeCall.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeCompound.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodeDoWhile.hpp"
#include "metron/nodes/CNodeEnum.hpp"
#include "metron/nodes/CNodeExpStatement.hpp"
#include "metron/nodes/CNodeExpression.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeFieldExpression.hpp"
#include "metron/nodes/CNodeFor.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeIf.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeLValue.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodeNamespace.hpp"
#include "metron/nodes/CNodePreproc.hpp"
#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeQualifiedIdentifier.hpp"
#include "metron/nodes/CNodeReturn.hpp"
#include "metron/nodes/CNodeStatement.hpp"
#include "metron/nodes/CNodeStruct.hpp"
#include "metron/nodes/CNodeSwitch.hpp"
#include "metron/nodes/CNodeTemplate.hpp"
#include "metron/nodes/CNodeTranslationUnit.hpp"
#include "metron/nodes/CNodeType.hpp"
#include "metron/nodes/CNodeTypedef.hpp"
#include "metron/nodes/CNodeUsing.hpp"
#include "metron/nodes/CNodeWhile.hpp"

#define TRACE_ERR(A) { LOG_R("bad bad %s : %s\n", typeid(*node).name(), A); assert(false); exit(-1); }

//------------------------------------------------------------------------------

Tracer::Tracer(CSourceRepo* repo, CInstance* root_inst, bool deep_trace, bool log_actions)
: repo(repo), root_inst(root_inst), deep_trace(deep_trace), log_actions(log_actions) {
}

void Tracer::reset() {
  cstack.clear();
  istack.clear();
}


Err Tracer::start_trace(CInstance* inst, CNodeFunction* func) {
  reset();
  cstack.push_back(func);
  istack.push_back(inst);
  Err err = trace_dispatch(func);

  // If we're tracing a top-level function with a return value, pretend that
  // the external environment reads that return value.
  /*
  if (auto inst_return = inst->resolve("@return")) {
    err << log_action(inst_return, nullptr, ACT_READ);
  }
  */

  return err;
}

bool Tracer::in_constructor() {
  for (int i = cstack.size() - 1; i >= 0; i--) {
    if (cstack[i]->as<CNodeConstructor>()) return true;
  }
  return false;
}

//------------------------------------------------------------------------------

Err Tracer::trace_dispatch(CNode* node) {
  if (node == nullptr) return Err();
  if (node->tag_noconvert()) return Err();

  if (auto n = node->as<CNodeAssignment>()) return trace(n);
  if (auto n = node->as<CNodeCall>()) return trace(n);
  if (auto n = node->as<CNodeFieldExpression>()) return trace(n);
  if (auto n = node->as<CNodeFor>()) return trace(n);
  if (auto n = node->as<CNodeIdentifier>()) return trace(n);
  if (auto n = node->as<CNodeIdentifierExp>()) return trace(n);
  if (auto n = node->as<CNodeIf>()) return trace(n);
  if (auto n = node->as<CNodePrefixExp>()) return trace(n);
  if (auto n = node->as<CNodeQualifiedIdentifier>()) return trace(n);
  if (auto n = node->as<CNodeReturn>()) return trace(n);
  if (auto n = node->as<CNodeSuffixExp>()) return trace(n);
  if (auto n = node->as<CNodeSwitch>()) return trace(n);

  return trace_children(node);
}

//------------------------------------------------------------------------------

Err Tracer::trace_children(CNode* node) {
  Err err;
  for (auto child : node) {
    err << trace_dispatch(child);
  }
  return err;
}

//------------------------------------------------------------------------------
// For "x[y] = z;", we don't want to allow two writes to x as that would imply
// two write ports on a block mem. So we log a read on x before the write if
// there's an array suffix.

Err Tracer::trace(CNodeAssignment* node) {
  Err err;

  auto lhs = node->child("lhs");
  auto op = node->child("op");
  auto rhs = node->child("rhs");

  err << trace_dispatch(rhs);

  //----------------------------------------
  // LHS is suffix exp

  if (auto lhs_suffix_exp = lhs->as<CNodeSuffixExp>()) {
    err << trace_dispatch(lhs->child("suffix"));

    auto inst_lhs = istack.back()->resolve(lhs);
    assert(inst_lhs);

    err << log_action(inst_lhs, node, ACT_READ);
    err << log_action(inst_lhs, node, ACT_WRITE);
  }

  //----------------------------------------
  // LHS is not suffix exp

  else {
    auto inst_lhs = istack.back()->resolve(lhs);
    if (!inst_lhs) return err;

    err << log_action(inst_lhs, node, ACT_WRITE);
  }

  //----------------------------------------

  return err;
}

//------------------------------------------------------------------------------
// FIXME not dealing with nested submod paths right now

Err Tracer::trace(CNodeCall* node) {
  Err err;

  auto src_class = node->ancestor<CNodeClass>();

  err << trace_dispatch(node->node_args);

  auto dst_name = node->node_name;

  if (auto field_exp = dst_name->as<CNodeFieldExpression>()) {
    auto submod_name = field_exp->child("field_path")->get_textstr();
    auto submod_field = src_class->get_field(submod_name);
    auto func_inst = istack.back()->resolve(field_exp)->as<CInstFunc>();
    auto node_func = func_inst->node_func;

    cstack.push_back(func_inst->node_func);
    istack.push_back(func_inst);

    for (auto child : func_inst->params) {
      err << log_action(child, node, ACT_WRITE);
    }

    if (deep_trace) {
      err << trace_dispatch(node_func);
    }

    if (func_inst->inst_return) {
      err << log_action(func_inst->inst_return, node, ACT_READ);
    }

    cstack.pop_back();
    istack.pop_back();

  } else {
    auto func_name = dst_name->get_textstr();
    auto dst_func = src_class->get_function(func_name);

    auto func_inst = istack.back()->resolve(func_name)->as<CInstFunc>();

    if (dst_func && func_inst) {

      cstack.push_back(dst_func);
      istack.push_back(func_inst);

      err << trace_dispatch(dst_func);

      cstack.pop_back();
      istack.pop_back();
    }
  }

  return err;
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodePrefixExp* node) {
  Err err;

  auto rhs = node->child("rhs");
  auto op = node->child("prefix")->get_text();

  err << trace_dispatch(rhs);

  if (op == "++" || op == "--") {
    auto inst_rhs = istack.back()->resolve(rhs);
    if (inst_rhs) {
      err << log_action(inst_rhs, node, ACT_WRITE);
    }
  }

  return err;
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeSuffixExp* node) {
  Err err;

  auto lhs = node->child("lhs");
  auto suffix = node->child("suffix");
  auto op = suffix->get_text();

  if (auto array_suffix = suffix->as<CNodeList>()) {
    err << trace_dispatch(array_suffix);
    return err;
  }

  err << trace_dispatch(lhs);

  if (op == "++" || op == "--") {
    auto inst_lhs = istack.back()->resolve(lhs);
    if (inst_lhs) {
      err << log_action(inst_lhs, node, ACT_WRITE);
    }
  }

  return err;
}

//------------------------------------------------------------------------------
// For statements do _not_ get traced in declaration order, as "step" comes
// last.

Err Tracer::trace(CNodeFor* node) {
  Err err;
  err << trace_dispatch(node->child("init"));
  err << trace_dispatch(node->child("condition"));
  err << trace_dispatch(node->child("body"));
  err << trace_dispatch(node->child("step"));
  return err;
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeIdentifierExp* node) {
  if (auto inst_field = istack.back()->resolve(node)) {
    return log_action(inst_field, node, ACT_READ);
  }
  return Err();
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeFieldExpression* node) {
  Err err;

  if (auto inst_dst = istack.back()->resolve(node)) {
    err << log_action(inst_dst, node, ACT_READ);
  }

  return err;
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeIdentifier* node) {
  if (auto inst_field = istack.back()->resolve(node)) {
    return log_action(inst_field, node, ACT_READ);
  }
  return Err();
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeIf* node) {
  Err err;

  auto body_true = node->child("body_true");
  auto body_false = node->child("body_false");

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

  err << trace_dispatch(node->child("condition"));

  root_inst->push_trace_state();
  if (body_true) err << trace_dispatch(body_true);
  root_inst->swap_trace_state();
  if (body_false) err << trace_dispatch(body_false);
  root_inst->merge_state();

  return err;
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeQualifiedIdentifier* node) {
  Err err;

  auto scope = node->child("scope_path")->name;
  auto field = node->child("identifier")->name;

  if (auto node_namespace = repo->get_namespace(scope)) {
    return trace_dispatch(node_namespace->get_field(field));
  }

  if (auto node_enum = repo->get_enum(scope)) {
    return Err();
  }

  if (auto node_class = node->ancestor<CNodeClass>()) {
    if (auto node_enum = node_class->get_enum(scope)) {
      return Err();
    }
  }

  TRACE_ERR("Don't know how to trace this qualified identifier");

  return err;
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeReturn* node) {
  Err err;

  if (auto node_value = node->child("value")) {
    err << trace_dispatch(node_value);
  }

  /*
  auto inst_return = istack.back()->resolve("@return");
  assert(inst_return);
  err << log_action(inst_return, node, ACT_WRITE);
  */

  return err;
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeSwitch* node) {
  Err err;

  err << trace_dispatch(node->node_cond);

  int case_count = 0;
  bool has_default = false;

  for (auto node_child : node->node_body) {
    // Skip cases without bodies
    if (!node_child->child("body")) continue;

    auto node_case = node_child->as<CNodeCase>();
    if (!node_case) continue;

    if (node_case->node_kwdefault) has_default = true;


    root_inst->push_trace_state();
    case_count++;
    err << trace_dispatch(node_child);
    root_inst->swap_trace_state();
  }

  if (has_default) {
    root_inst->pop_trace_state();
    case_count--;
  }

  for (int i = 0; i < case_count; i++) {
    root_inst->merge_state();
  }

  return err;
}

//------------------------------------------------------------------------------

Err Tracer::log_action2(CInstance* inst, CNode* node, TraceAction action) {
  Err err;

  if (in_constructor()) {
    // LOG_R("not recording action because we're inside init()\n");
    return err;
  }

  //----------------------------------------
  // We can see CInstFunc when we trace a function declaration, we don't have
  // to do anything here.

  if (auto inst_func = inst->as<CInstFunc>()) {
    return err;
  }

  //----------------------------------------
  // An action on a union applies to all the parts of the union.

  if (auto inst_union = inst->as<CInstUnion>()) {
    for (auto child : inst_union->parts) {
      err << log_action2(child, node, action);
    }
    return err;
  }

  //----------------------------------------
  // An action on a struct applies to all parts of the struct.

  if (auto inst_struct = inst->as<CInstStruct>()) {
    for (auto child : inst_struct->parts) {
      err << log_action2(child, node, action);
    }
    return err;
  }

  //----------------------------------------
  // An action on a primitive gets logged and added to self_reads/writes

  if (auto inst_prim = inst->as<CInstPrim>()) {
    assert(action == ACT_READ || action == ACT_WRITE);

    if (node) {
      auto func = node->ancestor<CNodeFunction>();
      assert(func);

      if (!deep_trace && !belongs_to_func(inst_prim)) {
        if (action == ACT_READ) {
          func->self_reads.insert(inst_prim);
        } else if (action == ACT_WRITE) {
          func->self_writes.insert(inst_prim);
        }
      }
    }

    //----------
    // Log action on primitive

    if (log_actions) {
      auto old_state = inst_prim->state_stack.back();
      auto new_state = merge_action(old_state, action);

      inst_prim->state_stack.back() = new_state;

      if (new_state == TS_INVALID) {
        LOG_R("Trace error: state went from %s to %s\n", to_string(old_state),
              to_string(new_state));
        err << ERR("Invalid context state\n");
      }
    }

    return err;
  }

  assert(false);
  return err;
}

//------------------------------------------------------------------------------

Err Tracer::log_action(CInstance* inst, CNode* node, TraceAction action) {

  std::function<CInstance*(CInstance*)> walk_up_unions;

  walk_up_unions = [&](CInstance* inst) -> CInstance* {
    if (inst->inst_parent) {
      auto parent_union = walk_up_unions(inst->inst_parent);
      if (parent_union) {
        return parent_union;
      }
    }

    if (inst->as<CInstUnion>()) {
      return inst;
    }

    return nullptr;
  };


  if (auto parent_union = walk_up_unions(inst)) {
    return log_action2(parent_union, node, action);
  }
  else {
    return log_action2(inst, node, action);
  }
}

//------------------------------------------------------------------------------
