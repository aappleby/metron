#include "metron/nodes/CNodeFunction.hpp"

#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeConstructor.hpp"
#include "metron/nodes/CNodeType.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeCompound.hpp"

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

bool CNodeFunction::should_emit_as_task() {
  bool called_by_tick = false;

  visit_internal_callers([&](CNodeFunction* f) {
    if (f->method_type == MT_TICK) called_by_tick = true;
  });

  return method_type == MT_TICK && called_by_tick;
}

//------------------------------------------------------------------------------

bool CNodeFunction::should_emit_as_func() {
  return method_type == MT_FUNC && internal_callers.size();
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
