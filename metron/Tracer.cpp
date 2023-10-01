#include "Tracer.hpp"

#include "metron/CInstance.hpp"
#include "metron/CSourceRepo.hpp"
#include "metron/Cursor.hpp"
#include "metron/nodes/CNodeAccess.hpp"
#include "metron/nodes/CNodeAssignment.hpp"
#include "metron/nodes/CNodeBuiltinType.hpp"
#include "metron/nodes/CNodeCall.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeCompound.hpp"
#include "metron/nodes/CNodeConstant.hpp"
#include "metron/nodes/CNodeConstructor.hpp"
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

typedef std::function<void(CNodeStatement*)> statement_visitor;

void visit_statements(CNode* node, statement_visitor visit_cb) {
  assert(node);

  if (auto node_compound = node->as<CNodeCompound>()) {
    for (auto statement : node_compound->statements) {
      visit_statements(statement, visit_cb);
    }
  }
  else if (auto node_if = node->as<CNodeIf>()) {
    if (node_if->node_then) visit_statements(node_if->node_then, visit_cb);
    if (node_if->node_else) visit_statements(node_if->node_else, visit_cb);
  }
  else if (auto node_for = node->as<CNodeFor>()) {
    visit_statements(node_for->child("body"), visit_cb);
  }
  else if (auto node_while = node->as<CNodeWhile>()) {
    visit_statements(node_while->child("body"), visit_cb);
  }
  else if (auto node_dowhile = node->as<CNodeDoWhile>()) {
    visit_statements(node_dowhile->child("body"), visit_cb);
  }
  else if (auto node_switch = node->as<CNodeSwitch>()) {
    for (auto node_case : node_switch->node_body->items) {
      visit_statements(node_case, visit_cb);
    }
  }
  else if (auto node_case = node->as<CNodeCase>()) {
    if (node_case->node_body) {
      for (auto node_statement : node_case->node_body) {
        visit_statements(node_statement, visit_cb);
      }
    }
  }
  else if (auto node_default = node->as<CNodeDefault>()) {
    if (node_default->node_body) {
      for (auto node_statement : node_default->node_body) {
        visit_statements(node_statement, visit_cb);
      }
    }
  }
  else {
    visit_cb(node->req<CNodeStatement>());
  }
}

//------------------------------------------------------------------------------

bool has_non_terminal_return(CNodeCompound* node_compound) {

  bool hit_return = false;
  bool bad_return = false;

  statement_visitor visitor = [&](CNodeStatement* node) {
    auto node_return = node->as<CNodeReturn>();
    if (node_return) {
      if (hit_return) bad_return = true;
      hit_return = true;
    }
    else if (hit_return) {
      bad_return = true;
    }
  };

  visit_statements(node_compound, visitor);

  return bad_return;
}

//------------------------------------------------------------------------------

bool ends_with_break(CNode* node) {
  if (!node) return false;
  if (node->as<CNodeKeyword>() && node->get_text() == "break") return true;

  if (auto node_compound = node->as<CNodeCompound>()) {
    for (auto i = 0; i < node_compound->statements.size() - 1; i++) {
      if (ends_with_break(node_compound->statements[i])) return false;
    }
    return ends_with_break(node_compound->statements.back());
  }

  if (auto node_if = node->as<CNodeIf>()) {
    if (node_if->node_else == nullptr) return false;
    return ends_with_break(node_if->node_then) &&
           ends_with_break(node_if->node_else);
  }

  if (auto node_list = node->as<CNodeList>()) {
    for (auto i = 0; i < node_list->items.size() - 1; i++) {
      if (ends_with_break(node_list->items[i])) return false;
    }
    return ends_with_break(node_list->items.back());
  }

  if (auto node_expstatement = node->as<CNodeExpStatement>()) {
    return ends_with_break(node_expstatement->node_exp);
  }

  return false;
}

//------------------------------------------------------------------------------

Err Tracer::trace_dispatch(CNode* node) {
  if (node == nullptr) return Err();

  if (auto n = node->as<CNodeBuiltinType>()) return Err();
  if (auto n = node->as<CNodeConstInt>()) return Err();
  if (auto n = node->as<CNodeConstString>()) return Err();
  if (auto n = node->as<CNodePunct>()) return Err();
  if (auto n = node->as<CNodeUsing>()) return Err();
  if (auto n = node->as<CNodeKeyword>()) return Err();

  if (auto n = node->as<CNodeAssignment>()) return trace(n);
  if (auto n = node->as<CNodeBinaryExp>()) return trace(n);
  if (auto n = node->as<CNodeCall>()) return trace(n);
  if (auto n = node->as<CNodeCase>()) return trace(n);
  if (auto n = node->as<CNodeCompound>()) return trace(n);
  if (auto n = node->as<CNodeDeclaration>()) return trace(n);
  if (auto n = node->as<CNodeDefault>()) return trace(n);
  if (auto n = node->as<CNodeExpStatement>()) return trace(n);
  if (auto n = node->as<CNodeField>()) return trace(n);
  if (auto n = node->as<CNodeFieldExpression>()) return trace(n);
  if (auto n = node->as<CNodeFor>()) return trace(n);
  if (auto n = node->as<CNodeFunction>()) return trace(n);
  if (auto n = node->as<CNodeIdentifier>()) return trace(n);
  if (auto n = node->as<CNodeIdentifierExp>()) return trace(n);
  if (auto n = node->as<CNodeIf>()) return trace(n);
  if (auto n = node->as<CNodeList>()) return trace(n);
  if (auto n = node->as<CNodeLValue>()) return trace(n);
  if (auto n = node->as<CNodePrefixExp>()) return trace(n);
  if (auto n = node->as<CNodeQualifiedIdentifier>()) return trace(n);
  if (auto n = node->as<CNodeReturn>()) return trace(n);
  if (auto n = node->as<CNodeSuffixExp>()) return trace(n);
  if (auto n = node->as<CNodeSwitch>()) return trace(n);

  LOG_R("Don't know how to trace a %s\n", typeid(*node).name());
  assert(false);
  return ERR("Don't know how to trace a %s\n", typeid(*node).name());
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

  auto rhs = node->child("rhs");
  auto op = node->child("op");
  auto lhs = node->child("lhs")->req<CNodeLValue>();
  auto suffix = lhs->child("suffix");

  err << trace_dispatch(rhs);

  if (suffix) {
    err << trace_dispatch(suffix);
  }

  auto inst_lhs = istack.back()->resolve(lhs->child("name"));
  if (!inst_lhs) return err;

  if (op->get_text() != "=" || suffix) {
    err << log_action(inst_lhs, node, ACT_READ);
  }

  err << log_action(inst_lhs, node, ACT_WRITE);

  return err;
}

//------------------------------------------------------------------------------
// FIXME not dealing with nested submod paths right now

Err Tracer::trace(CNodeCall* node) {
  Err err;

  auto src_class = node->ancestor<CNodeClass>();

  err << trace(node->node_args);

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
      err << trace(node_func);
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

      err << trace(dst_func);

      cstack.pop_back();
      istack.pop_back();
    }
  }

  return err;
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeCompound* node) {
  return trace_children(node);
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeDeclaration* node) {
  return trace_dispatch(node->node_value);
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeBinaryExp* node) {
  Err err;
  err << trace_dispatch(node->child("lhs"));
  err << trace_dispatch(node->child("rhs"));
  return err;
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodePrefixExp* node) {
  Err err;

  auto rhs = node->child("rhs");
  err << trace_dispatch(rhs);

  auto op = node->child("prefix")->get_text();
  if (op == "++" || op == "--") {
    auto inst_rhs = istack.back()->resolve(rhs);
    if (inst_rhs) {
      err << log_action(inst_rhs, node, ACT_READ);
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

  err << trace_dispatch(lhs);

  if (auto array_suffix = suffix->as<CNodeList>()) {
    err << trace(array_suffix);
    return err;
  }

  auto op = node->child("suffix")->get_text();
  if (op == "++" || op == "--") {
    auto inst_lhs = istack.back()->resolve(lhs);
    if (inst_lhs) {
      err << log_action(inst_lhs, node, ACT_READ);
      err << log_action(inst_lhs, node, ACT_WRITE);
    }
  }

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

Err Tracer::trace(CNodeExpStatement* node) {
  return trace_children(node);
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeField* node) { return trace(node->node_decl); }

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeFieldExpression* node) {
  Err err;

  auto inst_dst = istack.back()->resolve(node);

  err << log_action(inst_dst, node, ACT_READ);

  return err;
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeFor* node) {
  Err err;
  err << trace_dispatch(node->child("init"));
  err << trace_dispatch(node->child("condition"));
  err << trace_dispatch(node->child("body"));
  err << trace_dispatch(node->child("step"));
  return err;
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeFunction* node) {
  if (has_non_terminal_return(node->node_body)) {
    return ERR("Function has mid-block return");
  }

  return trace(node->node_body);
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

  root_inst->push_state();
  if (body_true) err << trace_dispatch(body_true);
  root_inst->swap_state();
  if (body_false) err << trace_dispatch(body_false);
  root_inst->merge_state();

  return err;
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeList* node) {
  return trace_children(node);
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeLValue* node) {
  return trace_children(node);
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeQualifiedIdentifier* node) {
  Err err;

  auto scope = node->child("scope_path")->get_name();
  auto field = node->child("identifier")->get_name();

  if (auto node_namespace = repo->get_namespace(scope)) {
    return trace(node_namespace->get_field(field));
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

  auto inst_return = istack.back()->resolve("@return");
  assert(inst_return);
  err << log_action(inst_return, node, ACT_WRITE);

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
    auto node_default = node_child->as<CNodeDefault>();
    if (!node_case && !node_default) continue;

    if (node_default) has_default = true;

    root_inst->push_state();
    case_count++;
    err << trace_dispatch(node_child);
    root_inst->swap_state();
  }

  if (has_default) {
    root_inst->pop_state();
    case_count--;
  }

  for (int i = 0; i < case_count; i++) {
    root_inst->merge_state();
  }

  return err;
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeCase* node) {
  if (node->node_body && !ends_with_break(node->node_body)) {
    return ERR(
        "All non-empty case statements must end with break (no fallthroughs)");
  }

  return trace(node->node_body);
}

//------------------------------------------------------------------------------

Err Tracer::trace(CNodeDefault* node) {
  if (node->node_body && !ends_with_break(node->node_body)) {
    return ERR(
        "All non-empty case statements must end with break (no fallthroughs)");
  }

  return trace(node->node_body);
}

//------------------------------------------------------------------------------

Err Tracer::log_action(CInstance* inst, CNode* node, TraceAction action) {
  Err err;

  if (auto inst_struct = inst->as<CInstStruct>()) {
    for (auto child : inst_struct->parts) {
      err << log_action(child, node, action);
    }
    return err;
  }

  if (auto inst_prim = inst->as<CInstPrim>()) {
    if (inst->name == "@return") {
      LOG("wat going on %d\n", action);
      return err;
    }

    assert(action == ACT_READ || action == ACT_WRITE);

    auto func = node->ancestor<CNodeFunction>();
    assert(func);

    if (!deep_trace) {
      if (action == ACT_READ) {
        func->self_reads.insert(inst_prim);
      } else if (action == ACT_WRITE) {
        func->self_writes.insert(inst_prim);
      }
    }

    if (in_constructor()) {
      // LOG_R("not recording action because we're inside init()\n");
      return err;
    }

    auto old_state = inst_prim->state_stack.back();
    auto new_state = merge_action(old_state, action);

    inst_prim->state_stack.back() = new_state;

    if (new_state == TS_INVALID) {
      LOG_R("Trace error: state went from %s to %s\n", to_string(old_state),
            to_string(new_state));
      err << ERR("Invalid context state\n");
    }

    return err;
  }

  assert(false);
  return err;
}

//------------------------------------------------------------------------------
