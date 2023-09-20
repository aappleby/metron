#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

#include "metrolib/core/Log.h"

#include <assert.h>
#include <vector>
#include <set>
#include <functional>

#include "NodeTypes.hpp"

struct CInstFunc;

//------------------------------------------------------------------------------

struct CNodeFunction : public CNode {

  void init(const char* match_tag, SpanType span, uint64_t flags);

  //----------------------------------------
  // Methods to be implemented by subclasses.

  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;

  //----------------------------------------

  CNodeClass* get_parent_class();
  std::string_view get_return_type_name() const;

  Err emit_init(Cursor& cursor);
  Err emit_always_comb(Cursor& cursor);
  Err emit_always_ff(Cursor& cursor);
  Err emit_func(Cursor& cursor);
  Err emit_task(Cursor& cursor);

  Err emit_func_binding_vars(Cursor& cursor);

  bool emit_as_task();
  bool emit_as_func();

#if 0
  needs_ports = is_public_ && internal_callers.empty();
#endif

  void dump();
  void dump_call_graph();

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
    auto name = get_name();
    assert(method_type == MT_UNKNOWN || method_type == new_type);
    method_type = new_type;
  }

  void propagate_rw() {
    for (auto c : internal_callees) c->propagate_rw();
    for (auto c : external_callees) c->propagate_rw();

    all_reads = self_reads;
    all_writes = self_writes;

    auto name = get_name();

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

    //needs_binding |= method_type == MT_TICK && called_by_tock();
    for (auto caller : internal_callers) {
      needs_binding |= method_type == MT_TICK && caller->method_type == MT_TOCK;
    }

    needs_binding |= method_type == MT_TOCK && !internal_callers.empty();
    return needs_binding;
  }

  bool has_return();

  //----------------------------------------

  bool called_by_init();

#if 0
  bool called_by_init() {
    bool called = false;
    visit_internal_callers([&](CNodeFunction* f) {
      called |= f->method_type == MT_INIT;
    });
    return called;
  }

  bool called_by_tick() {
    bool called = false;
    visit_internal_callers([&](CNodeFunction* f) {
      called |= f->method_type == MT_TICK;
    });
    return called;
  }

  bool called_by_tock() {
    bool called = false;
    visit_internal_callers([&](CNodeFunction* f) {
      called |= f->method_type == MT_TOCK;
    });
    return called;
  }

  bool called_by_func() {
    bool called = false;
    visit_internal_callers([&](CNodeFunction* f) {
      called |= f->method_type == MT_FUNC;
    });
    return called;
  }
#endif

  MethodType get_method_type();

  //----------------------------------------

  std::string name;

  CNodeType*       node_type   = nullptr;
  CNode*           node_name   = nullptr;
  CNodeList*       node_params = nullptr;
  CNodeList*       node_init   = nullptr;
  CNodeKeyword*    node_const  = nullptr;
  CNodeCompound*   node_body   = nullptr;

  MethodType method_type = MT_UNKNOWN;

  bool is_public = false;

  CInstFunc* instance = nullptr;

  //std::set<CNodeField*> self_reads;
  //std::set<CNodeField*> self_writes;

  //std::set<CNodeField*> all_reads;
  //std::set<CNodeField*> all_writes;

  std::set<CInstance*> self_reads;
  std::set<CInstance*> self_writes;

  std::set<CInstance*> all_reads;
  std::set<CInstance*> all_writes;

  std::vector<CNodeDeclaration*> params;

  std::set<CNodeFunction*> internal_callers;
  std::set<CNodeFunction*> internal_callees;
  std::set<CNodeFunction*> external_callers;
  std::set<CNodeFunction*> external_callees;

  // These are available after categorize_methods
  //std::set<CNodeFunction*> tick_callers;
  //std::set<CNodeFunction*> tock_callers;
  //std::set<CNodeFunction*> func_callers;

};

//------------------------------------------------------------------------------

struct CNodeConstructor : public CNodeFunction {
  void init(const char* match_tag, SpanType span, uint64_t flags);
};

//------------------------------------------------------------------------------
