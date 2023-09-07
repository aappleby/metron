#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

#include "metrolib/core/Log.h"

#include <assert.h>
#include <vector>
#include <set>
#include <functional>

struct CNodeClass;

//------------------------------------------------------------------------------

struct CNodeFunction : public CNode {

  void init(const char* match_tag, SpanType span, uint64_t flags);

  //----------------------------------------
  // Methods to be implemented by subclasses.

  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& c) override;
  CHECK_RETURN Err trace(CCall* call) override;

  //----------------------------------------

  CNodeClass* get_parent_class();
  std::string_view get_return_type_name() const;

  void dump();

  //bool is_public() const;
  //bool is_tick() const { assert(false); return false; }
  //bool is_tock() const { assert(false); return false; }

  //bool has_params() const;
  //bool has_return() const;

  using func_visitor = std::function<void(CNodeFunction*)>;

  inline void visit_internal_callees(func_visitor v) {
    for (auto f : internal_callees) {
      v(f);
      f->visit_internal_callees(v);
    }
  }

  void set_type(MethodType new_type) {
    assert(method_type == MT_UNKNOWN);
    method_type = new_type;
  }

  //----------------------------------------

  //MtModule* _mod = nullptr;
  //MnNode _node;
  //bool _public = false;
  //MtModLibrary* _lib = nullptr;
  //std::string _name;
  //MnNode _return_type;
  //TraceState state = CTX_NONE;

  //----------

  MethodType method_type = MT_UNKNOWN;

  bool is_public_ = false;

  //bool emit_as_always_comb = false;
  //bool emit_as_always_ff = false;
  //bool emit_as_init = false;
  //bool emit_as_task = false;
  //bool emit_as_func = false;
  //bool needs_binding = false;
  //bool needs_ports = false;

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
};

//------------------------------------------------------------------------------
