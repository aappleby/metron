#include "CNodeFunction.hpp"

#include "NodeTypes.hpp"

#include "metrolib/core/Log.h"
#include "matcheroni/Utilities.hpp"

using namespace matcheroni;

//------------------------------------------------------------------------------

uint32_t CNodeFunction::debug_color() const {
  return COL_ORANGE;
}

std::string_view CNodeFunction::get_name() const {
  return child("name")->get_name();
}

void CNodeFunction::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  node_type   = child("return_type")->req<CNodeType>();
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
    return as<CNodeConstructor>() ? emit_init(cursor) : emit_task(cursor);
  }
  else if (method_type == MT_TOCK) {
    return emit_always_comb(cursor);
  }
  else if (method_type == MT_TICK) {
    return called_by_tick ? emit_task(cursor) : emit_always_ff(cursor);
  }
  else if (method_type == MT_FUNC) {
    return internal_callers.size() ? emit_func(cursor) : emit_always_comb(cursor);
  }
  else {
    dump_tree();
    assert(false);
    return CNode::emit(cursor);
  }
}

//----------------------------------------

Err CNodeFunction::emit_init(Cursor& cursor) {
  Err err;

  // FIXME not using node_init yet

  for (auto param : node_params) {
    auto decl = param->as<CNodeDeclaration>();
    if (!decl) continue;
    err << cursor.start_line();
    err << cursor.emit_print("parameter ");
    err << cursor.emit_splice(decl->child("name"));
    err << cursor.emit_print(" = ");

    auto constructor_param_value = decl->child("value");
    if (!constructor_param_value) {
      LOG_R("All constructor params must have default values\n");
      assert(false);
    }

    err << cursor.emit_splice(constructor_param_value);
    err << cursor.emit_print(";");
  }

  err << cursor.start_line();
  err << cursor.emit_print("initial ");
  err << cursor.skip_to(node_body);
  err << node_body->emit_block(cursor, "begin", "end");
  err << cursor.emit_gap();

  return err << cursor.check_done(this);
}

//----------------------------------------

Err CNodeFunction::emit_always_comb(Cursor& cursor) {
  Err err;

  auto func_name = get_namestr();

  cursor.id_map.push(cursor.id_map.top());
  for (auto node_param : node_params) {
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

  err << cursor.comment_out(node_const);
  err << cursor.emit_gap();

  err << node_body->emit_block(cursor, "", "end");
  err << cursor.emit_gap();

  cursor.id_map.pop();

  return err << cursor.check_done(this);
}

//----------------------------------------

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

  err << cursor.comment_out(node_const);
  err << cursor.emit_gap();

  err << node_body->emit_block(cursor, "", "end");
  err << cursor.emit_gap();

  cursor.id_map.pop();

  return err << cursor.check_done(this);
}

//----------------------------------------

Err CNodeFunction::emit_func(Cursor& cursor) {
  Err err;

  err << cursor.emit_print("function ");

  err << cursor.emit(node_type);
  err << cursor.emit_gap();

  err << cursor.emit(node_name);
  err << cursor.emit_gap();

  err << cursor.emit(node_params);
  err << cursor.emit_print(";");
  err << cursor.emit_gap();

  err << cursor.comment_out(node_const);
  err << cursor.emit_gap();

  err << node_body->emit_block(cursor, "", "endfunction");
  err << cursor.emit_gap();

  return err << cursor.check_done(this);
}

//----------------------------------------

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

  err << cursor.comment_out(node_const);
  err << cursor.emit_gap();

  err << node_body->emit_block(cursor, "", "endtask");
  err << cursor.emit_gap();

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

Err CNodeFunction::trace(CCall* call) {
  return child("body")->trace(call);
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

void CNodeFunction::dump() {
  //dump_tree();

  auto name = get_name();

  LOG_S("Method \"%.*s\" ", name.size(), name.data());

  if (is_public_)  LOG_G("public ");
  if (!is_public_) LOG_G("private ");
  LOG_G("%s ", to_string(method_type));
  LOG_G("\n");

  if (params.size()) {
    LOG_INDENT_SCOPE();
    for (auto p : params) {
      auto text = p->child("type")->get_text();
      LOG_G("Param %.*s : %.*s", name.size(), name.data(), text.size(), text.data());

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
      auto name = r->get_name();
      LOG_G("Directly reads  %.*s : %s\n", name.size(), name.data(), to_string(r->field_type));
    }
  }

  if (self_writes.size()) {
    LOG_INDENT_SCOPE();
    for (auto w : self_writes) {
      auto name = w->get_name();
      LOG_G("Directly writes %.*s : %s\n", name.size(), name.data(), to_string(w->field_type));
    }
  }

  if (all_reads.size()) {
    LOG_INDENT_SCOPE();
    for (auto r : all_reads) {
      if (self_reads.contains(r)) continue;
      auto name = r->get_name();
      LOG_G("Indirectly reads  %.*s : %s\n", name.size(), name.data(), to_string(r->field_type));
    }
  }

  if (all_writes.size()) {
    LOG_INDENT_SCOPE();
    for (auto w : all_writes) {
      if (self_writes.contains(w)) continue;
      auto name = w->get_name();
      LOG_G("Indirectly writes %.*s : %s\n", name.size(), name.data(), to_string(w->field_type));
    }
  }

#if 0
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
#endif

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
