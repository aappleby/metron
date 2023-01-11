#pragma once

#include <set>
#include <string>
#include <vector>

#include "MtNode.h"

struct MtModule;
struct MtModLibrary;
struct MtField;
struct MtContext;

//------------------------------------------------------------------------------

enum MethodType {
  MT_INVALID = 0,
  MT_CONSTRUCTOR,
  MT_INIT,
  MT_FUNC,
  MT_TICK,
  MT_TOCK
};

struct MtMethod {
  MtMethod(MtModule* mod, MnNode n, bool is_public);
  ~MtMethod();

  const char* cname() const;
  std::string name() const;
  bool is_constructor() const;

  bool categorized() const;
  bool is_valid() const;
  bool is_public() const { return _public; }

  void dump();
  void dump_call_graph();

  bool has_params() const { return !param_nodes.empty(); }
  bool has_return() const { return !_type.is_null() && _type.text() != "void"; }

  bool has_param(const std::string& name) {
    for (const auto& p : param_nodes) {
      if (p.name4() == name) return true;
    }
    return false;
  }

  bool called_in_module() const {
    return !internal_callers.empty();
  }

  bool called_in_init() const {
    for (auto m : internal_callers) {
      if (m->is_constructor()) return true;
      if (m->called_in_init()) return true;
    }
    return false;
  }

  bool called_in_tick() const {
    for (auto m : internal_callers) {
      if (m->is_tick_) return true;
      if (m->called_in_tick()) return true;
    }
    return false;
  }

  bool called_in_tock() const {
    for (auto m : internal_callers) {
      if (m->is_tock_) return true;
      if (m->called_in_tock()) return true;
    }
    return false;
  }

  bool called_by_tock() const {
    for (auto m : internal_callers) {
      if (m->is_tock_) return true;
    }
    return false;
  }

  bool called_in_func() const {
    for (auto m : internal_callers) {
      if (m->is_tock_) return true;
      if (m->called_in_tock()) return true;
    }
    return false;
  }

  //----------------------------------------

  MtModule* _mod = nullptr;
  MnNode _node;
  bool _public = false;
  MtModLibrary* _lib = nullptr;
  std::string _name;
  MnNode _type;
  ContextState state = CTX_NONE;

  //----------

  MethodType method_type = MT_INVALID;
  bool is_init_ = false;
  bool is_tick_ = false;
  bool is_tock_ = false;
  bool is_func_ = false;
  bool is_toplevel = false;

  bool emit_as_always_comb = false;
  bool emit_as_always_ff = false;
  bool emit_as_init = false;
  bool emit_as_task = false;
  bool emit_as_func = false;
  bool needs_binding = false;
  bool needs_ports = false;

  std::vector<MnNode> param_nodes;

  std::set<MtMethod*> internal_callers;
  std::set<MtMethod*> internal_callees;

  std::set<MtMethod*> external_callers;
  std::set<MtMethod*> external_callees;

  // These are available after categorize_methods
  std::set<MtMethod*> tick_callers;
  std::set<MtMethod*> tock_callers;
  std::set<MtMethod*> func_callers;

  std::set<MtContext*> writes;
};

//------------------------------------------------------------------------------
