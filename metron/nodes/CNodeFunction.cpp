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

bool CNodeFunction::called_by_constructor() {
  for (auto c : internal_callers) {
    if (c->as<CNodeConstructor>()) return true;
    if (c->called_by_constructor()) return true;
  }
  return false;
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
// If an internal always_ff is called by an always_comb, we need to generate
// binding variables for its params.

bool CNodeFunction::needs_binding() {
  if (this->method_type == MT_ALWAYS_FF) {
    for (auto caller : internal_callers) {
      if (caller->method_type == MT_ALWAYS_COMB) return true;
    }
  }

  return false;
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

void CNodeFunction::propagate_rw() {
  for (auto f : internal_callees) {
    f->propagate_rw();
    tree_writes.insert(f->self_writes.begin(), f->self_writes.end());
    tree_writes.insert(f->tree_writes.begin(), f->tree_writes.end());
    tree_reads.insert (f->self_reads.begin(),  f->self_reads.end());
    tree_reads.insert (f->tree_reads.begin(),  f->tree_reads.end());
  }
}

//------------------------------------------------------------------------------
