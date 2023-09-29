#include "CNodeSwitch.hpp"

//==============================================================================
//==============================================================================

void CNodeSwitch::init(const char* match_tag, SpanType span, uint64_t flags) {
  node_condition = child("condition")->req<CNodeList>();
  node_body = child("body")->req<CNodeList>();

  assert(node_condition);
  assert(node_body);
}

//----------------------------------------

CHECK_RETURN Err CNodeSwitch::trace(CInstance* inst, call_stack& stack) {
  Err err;

  err << node_condition->trace(inst, stack);

  auto root_inst = inst->get_root();

  int case_count = 0;
  bool has_default = false;

  for (auto node_child : node_body) {
    // Skip cases without bodies
    if (!node_child->child("body")) continue;

    auto node_case = node_child->as<CNodeCase>();
    auto node_default = node_child->as<CNodeDefault>();
    if (!node_case && !node_default) continue;

    if (node_default) has_default = true;

    root_inst->push_state();
    case_count++;
    err << node_child->trace(inst, stack);
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

//----------------------------------------

CHECK_RETURN Err CNodeSwitch::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_switch = child("switch");
  auto node_cond   = child("condition");
  auto node_body   = child("body");

  err << cursor.emit_replacement(node_switch, "case");
  err << cursor.emit_gap();
  err << cursor.emit(node_cond);
  err << cursor.emit_gap();

  for (auto child : node_body) {
    if (child->tag_is("ldelim")) {
      err << cursor.skip_over(child);
    }
    else if (child->tag_is("rdelim")) {
      err << cursor.emit_replacement(child, "endcase");
    }
    else {
      err << cursor.emit(child);
    }

    if (child->node_next) err << cursor.emit_gap();
  }

  return err << cursor.check_done(this);
}

//==============================================================================
//==============================================================================




















//==============================================================================
//==============================================================================

void CNodeCase::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  node_case  = child("case")->req<CNodeKeyword>();
  node_cond  = child("condition")->req<CNode>();
  node_colon = child("colon")->req<CNodePunct>();
  node_body  = child("body")->opt<CNodeList>();
}

//----------------------------------------

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
    return ends_with_break(node_if->node_then) && ends_with_break(node_if->node_else);
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

//----------------------------------------

CHECK_RETURN Err CNodeCase::trace(CInstance* inst, call_stack& stack) {
  if (node_body && !ends_with_break(node_body)) {
    return ERR("All non-empty case statements must end with break (no fallthroughs)");
  }

  return node_body->trace(inst, stack);
}

//----------------------------------------

CHECK_RETURN Err CNodeCase::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  err << cursor.skip_over(node_case);
  err << cursor.skip_gap();
  err << cursor.emit(node_cond);
  err << cursor.emit_gap();

  if (node_body) {
    err << cursor.emit(node_colon);
    err << cursor.emit_gap();
    err << cursor.emit(node_body);
  }
  else {
    err << cursor.emit_replacement(node_colon, ",");
  }

  return err << cursor.check_done(this);
}

//==============================================================================
//==============================================================================





















//==============================================================================
//==============================================================================

void CNodeDefault::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  node_default = child("default")->req<CNodeKeyword>();
  node_colon   = child("colon")->req<CNodePunct>();
  node_body    = child("body")->opt<CNodeList>();
}


CHECK_RETURN Err CNodeDefault::trace(CInstance* inst, call_stack& stack) {
  return child("body")->trace(inst, stack);
}

CHECK_RETURN Err CNodeDefault::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto node_default = child("default");
  auto node_colon   = child("colon");
  auto node_body    = child("body");

  err << cursor.emit(node_default);
  err << cursor.emit_gap();

  if (node_body) {
    err << cursor.emit(node_colon);
    err << cursor.emit_gap();
    err << cursor.emit(node_body);
  }
  else {
    err << cursor.emit_replacement(node_colon, ",");
  }

  return err << cursor.check_done(this);
}

//==============================================================================
//==============================================================================
