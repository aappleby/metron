#include "metron/nodes/CNodeFunction.hpp"

#include "metron/NodeTypes.hpp"

#include "metrolib/core/Log.h"
#include "matcheroni/Utilities.hpp"

using namespace matcheroni;

typedef std::function<void(CNodeStatement*)> statement_visitor;

//------------------------------------------------------------------------------

void CNodeFunction::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  node_type   = child("return_type")->req<CNodeType>();
  node_name   = child("name")->req<CNodeIdentifier>();
  node_params = child("params")->req<CNodeList>();
  node_init   = child("init")->opt<CNodeList>();
  node_const  = child("const")->opt<CNodeKeyword>();
  node_body   = child("body")->req<CNodeCompound>();

  name = node_name->get_textstr();

  for (auto c : child("params")) {
    if (auto param = c->as<CNodeDeclaration>()) {
      params.push_back(param);
    }
  }
}

//------------------------------------------------------------------------------

uint32_t CNodeFunction::debug_color() const {
  return COL_ORANGE;
}

std::string_view CNodeFunction::get_name() const {
  return child("name")->get_name();
}

//------------------------------------------------------------------------------

Err CNodeFunction::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  bool called_by_init = false;
  bool called_by_tick = false;
  bool called_by_tock = false;
  bool called_by_func = false;

  visit_internal_callers([&](CNodeFunction* f) {
    if (f->method_type == MT_INIT) called_by_init = true;
    if (f->method_type == MT_TICK) called_by_tick = true;
    if (f->method_type == MT_TOCK) called_by_tock = true;
    if (f->method_type == MT_FUNC) called_by_func = true;
  });

  //----------

  if (method_type == MT_INIT) {
    err << (as<CNodeConstructor>() ? emit_init(cursor) : emit_task(cursor));
  }
  else if (method_type == MT_TOCK) {
    err << emit_always_comb(cursor);
  }
  else if (method_type == MT_TICK) {
    err << (called_by_tick ? emit_task(cursor) : emit_always_ff(cursor));
  }
  else if (method_type == MT_FUNC) {
    err << (internal_callers.size() ? emit_func(cursor) : emit_always_comb(cursor));
  }
  else {
    dump_tree();
    assert(false);
  }


  //----------

  if (needs_binding()) {
    err << emit_func_binding_vars(cursor);
  }

  return err;
}

//------------------------------------------------------------------------------

Err CNodeFunction::emit_init(Cursor& cursor) {
  Err err;

  // FIXME not using node_init yet

  for (auto param : node_params) {
    auto decl = param->as<CNodeDeclaration>();
    if (!decl) continue;

    auto decl_const = decl->child("const");
    auto decl_type  = decl->child("type");
    auto decl_name  = decl->child("name");
    auto decl_eq    = decl->child("eq");
    auto decl_value = decl->child("value");

    assert(decl_type);
    assert(decl_name);
    assert(decl_eq);
    assert(decl_value);

    auto old_cursor = cursor.tok_cursor;
    if (decl_const) {
      cursor.tok_cursor = decl_const->tok_begin();
    }
    else {
      cursor.tok_cursor = decl_type->tok_begin();
    }

    err << cursor.start_line();
    err << cursor.emit_print("parameter ");

    if (decl_const) {
      err << cursor.emit(decl_const);
      err << cursor.emit_gap();
    }

    err << cursor.skip_over(decl_type);
    err << cursor.skip_gap();

    err << cursor.emit(decl_name);
    err << cursor.emit_gap();

    err << cursor.emit(decl_eq);
    err << cursor.emit_gap();

    err << cursor.emit(decl_value);
    err << cursor.emit_print(";");

    cursor.tok_cursor = old_cursor;
  }

  err << cursor.start_line();
  err << cursor.emit_print("initial ");
  err << cursor.skip_to(node_body);
  err << node_body->emit_block(cursor, "begin", "end");

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

Err CNodeFunction::emit_always_comb(Cursor& cursor) {
  Err err;

  auto func_name = get_namestr();

  cursor.id_map.push(cursor.id_map.top());
  for (auto node_param : node_params->items) {
    auto param = node_param->as<CNodeDeclaration>();
    if (!param) continue;

    auto param_name = param->get_namestr();
    cursor.id_map.top()[param_name] = func_name + "_" + param_name;
  }

  err << cursor.emit_replacement(node_type, "always_comb begin :");
  err << cursor.emit_gap();
  err << cursor.emit(node_name);
  err << cursor.emit_gap();

  err << cursor.skip_over(node_params);
  err << cursor.emit_gap();

  if (node_const) {
    err << cursor.skip_over(node_const);
    err << cursor.skip_gap();
  }

  err << node_body->emit_block(cursor, "", "end");

  cursor.id_map.pop();

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

Err CNodeFunction::emit_always_ff(Cursor& cursor) {
  Err err;

  auto func_name = get_namestr();

  cursor.id_map.push(cursor.id_map.top());
  for (auto node_param : node_params) {
    auto param = node_param->as<CNodeDeclaration>();
    if (!param) continue;

    auto param_name = param->get_namestr();
    cursor.id_map.top()[param_name] = func_name + "_" + param_name;
  }

  err << cursor.emit_replacement(node_type, "always_ff @(posedge clock) begin :");
  err << cursor.emit_gap();
  err << cursor.emit(node_name);
  err << cursor.emit_gap();

  err << cursor.skip_over(node_params);
  err << cursor.emit_gap();

  if (node_const) {
    err << cursor.skip_over(node_const);
    err << cursor.skip_gap();
  }

  err << node_body->emit_block(cursor, "", "end");

  cursor.id_map.pop();

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

bool CNodeFunction::emit_as_task() {
  bool called_by_tick = false;

  visit_internal_callers([&](CNodeFunction* f) {
    if (f->method_type == MT_TICK) called_by_tick = true;
  });

  return method_type == MT_TICK && called_by_tick;
}

//------------------------------------------------------------------------------

bool CNodeFunction::emit_as_func() {
  return method_type == MT_FUNC && internal_callers.size();
}

//------------------------------------------------------------------------------

Err CNodeFunction::emit_func(Cursor& cursor) {
  Err err;

  err << cursor.emit_print("function ");

  err << cursor.emit(node_type);
  err << cursor.emit_gap();

  err << cursor.emit(node_name);
  err << cursor.emit_gap();

  err << cursor.emit(node_params);

  if (node_const) {
    err << cursor.emit_gap();
    err << cursor.comment_out(node_const);
  }

  err << cursor.emit_print(";");
  err << cursor.emit_gap();

  err << node_body->emit_block(cursor, "", "endfunction");

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

Err CNodeFunction::emit_task(Cursor& cursor) {
  Err err;

  err << cursor.emit_print("task automatic ");

  err << cursor.skip_over(node_type);
  err << cursor.skip_gap();

  err << cursor.emit(node_name);
  err << cursor.emit_gap();

  err << cursor.emit(node_params);
  err << cursor.emit_print(";");
  err << cursor.emit_gap();

  if (node_const) {
    err << cursor.comment_out(node_const);
    err << cursor.emit_gap();
  }

  err << node_body->emit_block(cursor, "", "endtask");

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err CNodeFunction::emit_func_binding_vars(Cursor& cursor) {
  Err err;

  for (auto& param : params) {

    auto param_type = param->child("type")->as<CNodeType>();
    auto param_name = param->child("name")->as<CNodeIdentifier>();

    err << cursor.start_line();
    err << cursor.emit_splice(param_type);
    err << cursor.emit_print(" %s_", name.c_str());
    err << cursor.emit_splice(param_name);
    err << cursor.emit_print(";");
  }

  if (node_type->get_text() != "void") {
    err << cursor.start_line();
    err << cursor.emit_splice(node_type);
    err << cursor.emit_print(" %s_ret;", name.c_str());
  }

  return err;
}

//------------------------------------------------------------------------------

inline void visit_statements(CNode* node, statement_visitor visit_cb) {
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

Err CNodeFunction::trace(CInstance* inst, call_stack& stack) {
  if (has_non_terminal_return(node_body)) {
    return ERR("Function has mid-block return");
  }

  return node_body->trace(inst, stack);
}

//------------------------------------------------------------------------------

CNodeClass* CNodeFunction::get_parent_class() {
  return ancestor<CNodeClass>();
}

std::string_view CNodeFunction::get_return_type_name() const {
  return child("return_type")->get_text();
}

//------------------------------------------------------------------------------

// FIXME constructor needs to be in internal_callers

void CNodeFunction::dump() const {
  //dump_tree();

  auto name = get_name();

  LOG_S("Method \"%.*s\" ", name.size(), name.data());

  if (is_public)  LOG_G("public ");
  if (!is_public) LOG_G("private ");
  LOG_G("%s ", to_string(method_type));
  LOG_G("\n");

  if (params.size()) {
    LOG_INDENT_SCOPE();
    for (auto p : params) {
      auto param_name = p->child("name")->get_textstr();
      auto param_type = p->child("type")->get_textstr();
      LOG_G("Param %s : %s", param_name.c_str(), param_type.c_str());

      if (auto val = p->child("value")) {
        auto text = val->get_text();
        LOG_G(" = %.*s", text.size(), text.data());
      }

      LOG_G("\n");
    }
  }

  if (self_reads.size()) {
    LOG_INDENT_SCOPE();
    for (auto r : self_reads) {
      LOG_G("Directly reads  %s : %s\n", r->get_path().c_str(), to_string(r->get_state()));
    }
  }

  if (self_writes.size()) {
    LOG_INDENT_SCOPE();
    for (auto w : self_writes) {
      LOG_G("Directly writes %s : %s\n", w->get_path().c_str(), to_string(w->get_state()));
    }
  }

  if (all_reads.size()) {
    LOG_INDENT_SCOPE();
    for (auto r : all_reads) {
      if (self_reads.contains(r)) continue;
      LOG_G("Indirectly reads  %s : %s\n", r->get_path().c_str(), to_string(r->get_state()));
    }
  }

  if (all_writes.size()) {
    LOG_INDENT_SCOPE();
    for (auto w : all_writes) {
      if (self_writes.contains(w)) continue;
      LOG_G("Indirectly writes %s : %s\n", w->get_path().c_str(), to_string(w->get_state()));
    }
  }

  if (internal_callers.size()) {
    LOG_INDENT_SCOPE();
    for (auto c : internal_callers) {
      auto func_name = c->get_name();
      auto class_name = c->get_parent_class()->get_name();
      LOG_V("Called by %.*s::%.*s\n", class_name.size(), class_name.data(), func_name.size(), func_name.data());
    }
  }

  if (external_callers.size()) {
    LOG_INDENT_SCOPE();
    for (auto c : external_callers) {
      auto func_name = c->get_name();
      auto class_name = c->get_parent_class()->get_name();
      LOG_V("Called by %.*s::%.*s\n", class_name.size(), class_name.data(), func_name.size(), func_name.data());
    }
  }

  if (internal_callees.size()) {
    LOG_INDENT_SCOPE();
    for (auto c : internal_callees) {
      auto func_name = c->get_name();
      auto class_name = c->get_parent_class()->get_name();
      LOG_T("Calls %.*s::%.*s\n", class_name.size(), class_name.data(), func_name.size(), func_name.data());
    }
  }

  if (external_callees.size()) {
    LOG_INDENT_SCOPE();
    for (auto c : external_callees) {
      auto func_name = c->get_name();
      auto class_name = c->get_parent_class()->get_name();
      LOG_T("Calls %.*s::%.*s\n", class_name.size(), class_name.data(), func_name.size(), func_name.data());
    }
  }
}

//------------------------------------------------------------------------------

void CNodeFunction::dump_call_graph() {

  for (auto r : self_reads) {
    LOG("Reads %s\n", r->get_path().c_str());
  }
  for (auto w : self_writes) {
    LOG("Writes %s\n", w->get_path().c_str());
  }

  if (internal_callees.size()) {
    for (auto c : internal_callees) {
      auto func_name = c->get_name();
      auto class_name = c->get_parent_class()->get_name();
      LOG_T("%.*s::%.*s\n", class_name.size(), class_name.data(), func_name.size(), func_name.data());
      LOG_INDENT();
      c->dump_call_graph();
      LOG_DEDENT();
    }
  }

  if (external_callees.size()) {
    for (auto c : external_callees) {
      auto func_name = c->get_name();
      auto class_name = c->get_parent_class()->get_name();
      LOG_T("%.*s::%.*s\n", class_name.size(), class_name.data(), func_name.size(), func_name.data());
      LOG_INDENT();
      c->dump_call_graph();
      LOG_DEDENT();
    }
  }
}

//------------------------------------------------------------------------------

bool CNodeFunction::called_by_init() {
  for (auto c : internal_callers) {
    if (c->called_by_init()) return true;
  }
  return as<CNodeConstructor>() != nullptr;
}

//------------------------------------------------------------------------------

MethodType CNodeFunction::get_method_type() {
  if (called_by_init()) return MT_INIT;
  if (name.starts_with("tick")) return MT_TICK;
  if (name.starts_with("tock")) return MT_TOCK;

  auto result = MT_UNKNOWN;

  if (all_writes.empty()) {
    return MT_FUNC;
  }

  for (auto c : internal_callers) {
    auto new_result = c->get_method_type();
    if (new_result == MT_UNKNOWN) {
      LOG_R("%s: get_method_type returned MT_UNKNOWN\n", name.c_str());
      assert(false);
    }

    if (result == MT_UNKNOWN) {
      result = new_result;
    }
    else if (result != new_result) {
      LOG_R("%s: get_method_type inconsistent\n", name.c_str());
      assert(false);
    }
  }

  if (result != MT_UNKNOWN) return result;

  // Function was not a tick or tock, and was not called by a tick or tock.

  if (self_writes.size()) {
    LOG_R("Function %s writes stuff but is not tick or tock\n", name.c_str());
    assert(false);
  }

  return result;
}

//------------------------------------------------------------------------------

bool CNodeFunction::has_return() {
  return node_type->get_text() != "void";
}

//------------------------------------------------------------------------------




















//------------------------------------------------------------------------------

void CNodeConstructor::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  //node_type   = child("return_type")->req<CNodeType>();
  node_type   = nullptr;
  node_name   = child("name")->req<CNodeIdentifier>();
  node_params = child("params")->req<CNodeList>();
  node_init   = child("init")->opt<CNodeList>();
  node_const  = child("const")->opt<CNodeKeyword>();
  node_body   = child("body")->req<CNodeCompound>();

  for (auto c : child("params")) {
    if (auto param = c->as<CNodeDeclaration>()) {
      params.push_back(param);
    }
  }
}

//------------------------------------------------------------------------------
