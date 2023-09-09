#include "CNodeFunction.hpp"

#include "CNodeClass.hpp"
#include "CNodeType.hpp"
#include "CNodeStatement.hpp"
#include "NodeTypes.hpp"
#include "CNodeDeclaration.hpp"

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

  for (auto c : child("params")) {
    if (auto param = c->as_a<CNodeDeclaration>()) {
      params.push_back(param);
    }
  }
}

//------------------------------------------------------------------------------

#if 0
CHECK_RETURN Err MtCursor::emit_sym_function_definition(MnNode n) {
  Err err = check_at(sym_function_definition, n);

  auto return_type = n.get_field(field_type);
  auto func_decl = n.get_field(field_declarator);

  current_method.push(current_mod.top()->get_method(n.name4()));
  assert(current_method.top());

  //----------
  // Emit a block declaration for the type of function we're in.

  if (current_method.top()->emit_as_always_comb) {
    err << emit_func_as_always_comb(n);
  }
  else if (current_method.top()->emit_as_always_ff) {
    err << emit_func_as_always_ff(n);
  }
  else if (current_method.top()->emit_as_init) {
    err << emit_func_as_init(n);
  }
  else if (current_method.top()->emit_as_task) {
    err << emit_func_as_task(n);
  }
  else if (current_method.top()->emit_as_func) {
    err << emit_func_as_func(n);
  }

  //----------

  if (current_method.top()->needs_binding) {
    err << emit_func_binding_vars(current_method.top());
  }

  current_method.pop();
  return err << check_done(n);
}
#endif

Err CNodeFunction::emit(Cursor& c) {
  Err err;

  if (method_type == MT_TOCK && internal_callers.empty()) {
    return emit_always_comb(c);
  }
  else if (method_type == MT_TICK && internal_callers.empty()) {
    return emit_always_ff(c);
  }
  else if (method_type == MT_FUNC && internal_callers.empty()) {
    return emit_always_comb(c);
  }

  err << CNode::emit(c);

  return err;
}

//----------------------------------------

Err CNodeFunction::emit_always_comb(Cursor& c) {
  Err err;

  auto node_type   = child("return_type")->as_a<CNodeType>();
  auto node_name   = child("name")->as_a<CNodeIdentifier>();
  auto node_params = child("params")->as_a<CNodeList>();
  auto node_body   = child("body")->as_a<CNodeCompound>();

  auto func_name = get_namestr();

  c.id_map.push(c.id_map.top());
  for (auto node_param : node_params) {
    auto param = node_param->as_a<CNodeDeclaration>();
    if (!param) continue;

    auto param_name = param->get_namestr();
    c.id_map.top()[param_name] = func_name + "_" + param_name;
  }

  err << c.emit_replacement(node_type, "always_comb begin :");
  err << c.emit_gap_after(node_type);
  err << c.emit(node_name);
  err << c.emit_gap_after(node_name);

  err << c.skip_over(node_params);
  err << c.emit_gap_after(node_params);

  err << node_body->emit_block(c, "", "end");
  err << c.emit_gap_after(node_body);

  c.id_map.pop();

  return err;
}

//----------------------------------------

/*
CHECK_RETURN Err MtCursor::emit_func_as_always_ff(MnNode n) {
  Err err;

  auto func_type = n.get_field(field_type);
  auto func_decl = n.get_field(field_declarator);
  auto func_body = n.get_field(field_body);
  auto func_params = func_decl.get_field(field_parameters);

  id_map.push(id_map.top());
  for (auto c : func_params) {
    if (c.sym == sym_parameter_declaration) {
      id_map.top()[c.name4()] = func_decl.name4() + "_" + c.name4();
    }
  }

  err << emit_replacement(func_type, "always_ff @(posedge clock) begin :");
  err << emit_gap(func_type, func_decl);
  err << emit_replacement(func_decl, func_decl.name4().c_str());
  err << emit_gap(func_decl, func_body);
  err << emit_block(func_body, "", "end");

  id_map.pop();
  return err << check_done(n);
}
*/

Err CNodeFunction::emit_always_ff(Cursor& c) {
  Err err;

  auto node_type   = child("return_type")->as_a<CNodeType>();
  auto node_name   = child("name")->as_a<CNodeIdentifier>();
  auto node_params = child("params")->as_a<CNodeList>();
  auto node_body   = child("body")->as_a<CNodeCompound>();

  auto func_name = get_namestr();

  c.id_map.push(c.id_map.top());
  for (auto node_param : node_params) {
    auto param = node_param->as_a<CNodeDeclaration>();
    if (!param) continue;

    auto param_name = param->get_namestr();
    c.id_map.top()[param_name] = func_name + "_" + param_name;
  }

  err << c.emit_replacement(node_type, "always_ff @(posedge clock) begin :");
  err << c.emit_gap_after(node_type);
  err << c.emit(node_name);
  err << c.emit_gap_after(node_name);

  err << c.skip_over(node_params);
  err << c.emit_gap_after(node_params);

  err << node_body->emit_block(c, "", "end");
  err << c.emit_gap_after(node_body);

  c.id_map.pop();

  return err;
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
