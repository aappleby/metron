#pragma once

#include "metron/CNode.hpp"
#include "metron/Cursor.hpp"

#include "metrolib/core/Log.h"

#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeType.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeList.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeCompound.hpp"

#include <assert.h>
#include <vector>
#include <set>
#include <functional>

struct CInstFunc;
struct CNodeType;
struct CNode;
struct CNodeList;
struct CNodeList;
struct CNodeKeyword;
struct CNodeCompound;
struct CNodeDeclaration;

//------------------------------------------------------------------------------

struct CNodeFunction : public CNode {

  void init() {
    node_type   = child("return_type")->req<CNodeType>();
    node_name   = child("name")->req<CNodeIdentifier>();
    node_params = child("params")->req<CNodeList>();
    node_init   = child("init")->opt<CNodeList>();
    node_const  = child("const")->opt<CNodeKeyword>();
    node_body   = child("body")->req<CNodeCompound>();

    name = node_name->name;

    for (auto c : child("params")) {
      if (auto param = c->as<CNodeDeclaration>()) {
        params.push_back(param);
      }
    }
    color = COL_ORANGE;
  }

  //------------------------------------------------------------------------------

  bool should_emit_as_task() {
    bool called_by_tick = false;

    visit_internal_callers([&](CNodeFunction* f) {
      if (f->method_type == MT_TICK) called_by_tick = true;
    });

    return method_type == MT_TICK && called_by_tick;
  }

  //------------------------------------------------------------------------------

  bool should_emit_as_func() {
    return method_type == MT_FUNC && internal_callers.size();
  }

  //------------------------------------------------------------------------------

  bool called_by_init();

  //------------------------------------------------------------------------------

  bool has_return() {
    return node_type->get_text() != "void";
  }

  using func_visitor = std::function<void(CNodeFunction*)>;

  inline void visit_internal_callees(func_visitor v) {
    for (auto f : internal_callees) {
      v(f);
      f->visit_internal_callees(v);
    }
  }

  inline void visit_external_callees(func_visitor v) {
    for (auto f : external_callees) {
      v(f);
      f->visit_external_callees(v);
    }
  }

  inline void visit_internal_callers(func_visitor v) {
    for (auto f : internal_callers) {
      v(f);
      f->visit_internal_callers(v);
    }
  }

  inline void visit_external_callers(func_visitor v) {
    for (auto f : external_callers) {
      v(f);
      f->visit_external_callers(v);
    }
  }

  void set_type(MethodType new_type) {
    assert(method_type == MT_UNKNOWN || method_type == new_type);
    method_type = new_type;
  }

  void propagate_rw() {
    for (auto c : internal_callees) c->propagate_rw();
    for (auto c : external_callees) c->propagate_rw();

    all_reads = self_reads;
    all_writes = self_writes;

    for (auto c : internal_callees) {
      all_reads.insert(c->all_reads.begin(), c->all_reads.end());
      all_writes.insert(c->all_writes.begin(), c->all_writes.end());
    }
    for (auto c : external_callees) {
      all_reads.insert(c->all_reads.begin(), c->all_reads.end());
      all_writes.insert(c->all_writes.begin(), c->all_writes.end());
    }
  }

  bool needs_binding() {
    bool needs_binding = false;

    for (auto caller : internal_callers) {
      needs_binding |= method_type == MT_TICK && caller->method_type == MT_TOCK;
    }

    needs_binding |= method_type == MT_TOCK && !internal_callers.empty();
    return needs_binding;
  }

  void collect_writes2() {
    all_writes2.insert(self_writes2.begin(), self_writes2.end());
    for (auto f : internal_callees) {
      f->collect_writes2();
      all_writes2.insert(f->self_writes2.begin(), f->self_writes2.end());
    }
    for (auto f : external_callees) {
      f->collect_writes2();
      all_writes2.insert(f->self_writes2.begin(), f->self_writes2.end());
    }
  }

  //----------------------------------------

  CNodeType*       node_type   = nullptr;
  CNode*           node_name   = nullptr;
  CNodeList*       node_params = nullptr;
  CNodeList*       node_init   = nullptr;
  CNodeKeyword*    node_const  = nullptr;
  CNodeCompound*   node_body   = nullptr;

  MethodType method_type = MT_UNKNOWN;

  bool is_public = false;

  CInstFunc* instance = nullptr;

  std::set<CInstance*> self_reads;
  std::set<CInstance*> self_writes;

  std::set<CInstance*> all_reads;
  std::set<CInstance*> all_writes;

  std::set<CNodeField*> self_writes2;
  std::set<CNodeField*> all_writes2;

  std::vector<CNodeDeclaration*> params;

  std::set<CNodeFunction*> internal_callers;
  std::set<CNodeFunction*> internal_callees;
  std::set<CNodeFunction*> external_callers;
  std::set<CNodeFunction*> external_callees;
};

//------------------------------------------------------------------------------
