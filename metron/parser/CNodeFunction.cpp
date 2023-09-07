#include "CNodeFunction.hpp"

#include "CNodeClass.hpp"
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
  return CNode::emit(c);
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

/*
  bool is_public_ = false;
  bool is_init_ = false;
  bool is_tick_ = false;
  bool is_tock_ = false;
  bool is_func_ = false;

  std::set<CNodeFunction*> internal_callers;
  std::set<CNodeFunction*> internal_callees;
  std::set<CNodeFunction*> external_callers;
  std::set<CNodeFunction*> external_callees;
*/

// FIXME constructor needs to be in internal_callers

// FIXME function should pull out its list of args

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
