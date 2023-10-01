#include "metron/nodes/CNodeFunction.hpp"

#include "metrolib/core/Log.h"
#include "matcheroni/Utilities.hpp"
#include "metron/Emitter.hpp"
#include "metron/nodes/CNodeStatement.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeConstructor.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodeType.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeCompound.hpp"
#include "metron/nodes/CNodeIf.hpp"
#include "metron/nodes/CNodeFor.hpp"
#include "metron/nodes/CNodeWhile.hpp"
#include "metron/nodes/CNodeDoWhile.hpp"
#include "metron/nodes/CNodeCompound.hpp"
#include "metron/nodes/CNodeSwitch.hpp"
#include "metron/nodes/CNodeReturn.hpp"

using namespace matcheroni;

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
