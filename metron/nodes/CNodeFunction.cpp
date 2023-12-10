#include "metron/nodes/CNodeFunction.hpp"

#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeType.hpp"
#include "metron/nodes/CNodeCompound.hpp"
#include "metron/CInstance.hpp"

#include "metrolib/core/Log.h"

#include <assert.h>

//------------------------------------------------------------------------------

void CNodeFunction::init() {
  node_type   = child("return_type")->opt<CNodeType>();
  node_name   = child("name")->req<CNodeIdentifier>();
  node_params = child("params")->req<CNodeList>();
  node_init   = child("init")->opt<CNodeList>();
  node_const  = child("const")->opt<CNodeKeyword>();
  node_body   = child("body")->opt<CNodeCompound>();
  node_semi   = child("semi")->opt<CNodePunct>();

  name = node_name->name;

  for (auto c : child("params")) {
    if (auto param = c->as<CNodeDeclaration>()) {
      params.push_back(param);
    }
  }
}

//------------------------------------------------------------------------------

void CNodeFunction::update_type() {
  if (method_type != MT_UNKNOWN) return;

  for (auto f : internal_callees) f->update_type();
  for (auto f : external_callees) f->update_type();

  // If we're a constructor, we're an init.
  if (as<CNodeConstructor>()) {
    set_type(MT_INIT);
    return;
  }

  // If we're called by a constructor and we write something, we're an init and
  // can't be anything else.
  if (!self_writes.empty() && called_by_init()) {
    set_type(MT_INIT);
    return;
  }

  // If we write a signal, we're a tock. If we write a state, we'rea tick.

  if (!self_writes.empty()) {
    for (auto w : self_writes) {
      if      (w->is_reg())  set_type(MT_TICK);
      else if (w->is_sig()) set_type(MT_TOCK);
      else                     assert(false);
    }
  }

  // If we call external methods, we're a tock.
  if (external_callees.size()) {
    set_type(MT_TOCK);
  }

  // If we call an internal tock, we're a tock.
  for (auto f : internal_callees) {
    if (f->method_type == MT_TOCK) set_type(MT_TOCK);
  }

  // If we're not categorized yet and we call an internal tick, we're a tick.
  // Unless we have a return value, in which case we're a tock.
  if (method_type == MT_UNKNOWN) {
    for (auto f : internal_callees) {
      if (f->method_type == MT_TICK) {
        if (has_return()) {
          set_type(MT_TOCK);
        }
        else {
          set_type(MT_TICK);
        }
      }
    }
  }

  // If we're still not categorized then we don't call any tocks or ticks - we're a func.
  if (method_type == MT_UNKNOWN) {
    for (auto f : internal_callees) {
      assert (f->method_type == MT_FUNC);
    }
    set_type(MT_FUNC);
  }
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

bool CNodeFunction::has_return() {
  return node_type->get_text() != "void";
}

//------------------------------------------------------------------------------

void CNodeFunction::visit_internal_callees(func_visitor v) {
  for (auto f : internal_callees) {
    v(f);
    f->visit_internal_callees(v);
  }
}

void CNodeFunction::visit_external_callees(func_visitor v) {
  for (auto f : external_callees) {
    v(f);
    f->visit_external_callees(v);
  }
}

void CNodeFunction::visit_internal_callers(func_visitor v) {
  for (auto f : internal_callers) {
    v(f);
    f->visit_internal_callers(v);
  }
}

void CNodeFunction::visit_external_callers(func_visitor v) {
  for (auto f : external_callers) {
    v(f);
    f->visit_external_callers(v);
  }
}

//------------------------------------------------------------------------------

void CNodeFunction::set_type(MethodType new_type) {
  assert(method_type == MT_UNKNOWN || method_type == new_type);
  method_type = new_type;
}

//------------------------------------------------------------------------------

bool CNodeFunction::needs_binding() {
  bool needs_binding = false;

  for (auto caller : internal_callers) {
    needs_binding |= method_type == MT_TICK && caller->method_type == MT_TOCK;
  }

  needs_binding |= method_type == MT_TOCK && !internal_callers.empty();
  return needs_binding;
}

//------------------------------------------------------------------------------

bool CNodeFunction::must_call_before(CNodeFunction* func) {
  for (auto f : this->internal_callees) if (f->must_call_before(func)) return true;
  for (auto f : func->internal_callees) if (this->must_call_before(f)) return true;

  for (auto r : self_reads) {
    for (auto w : func->self_writes) {
      if (r == w && r->is_reg()) return true;
    }
  }

  for (auto w : self_writes) {
    for (auto r : func->self_reads) {
      if (w == r && r->is_sig()) return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------

bool CNodeFunction::called_by_init() {
  for (auto c : internal_callers) {
    if (c->called_by_init()) return true;
  }
  return as<CNodeConstructor>() != nullptr;
}

//------------------------------------------------------------------------------
