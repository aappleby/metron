#include "CNodeFunction.hpp"

#include "CNodeClass.hpp"
#include "CNodeStatement.hpp"
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
  auto name = get_name();

  LOG_S("Method \"%.*s\" ", name.size(), name.data());

  if (is_public_)  LOG_G("public ");
  if (!is_public_) LOG_G("private ");
  if (is_init_)    LOG_G("is_init_ ");
  if (is_tick_)    LOG_G("is_tick_ ");
  if (is_tock_)    LOG_G("is_tock_ ");
  if (is_func_)    LOG_G("is_func_ ");
  LOG_G("\n");

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
