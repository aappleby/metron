#pragma once

#include <set>
#include <string>
#include <vector>

#include "MtNode.h"

struct MtModule;
struct MtModLibrary;
struct MtContext;

//------------------------------------------------------------------------------

struct MtMethod {
  MtMethod(MtModule* mod, MnNode n, bool is_public);
  ~MtMethod();

  const char* cname() const;
  std::string name() const;
  bool is_constructor() const;

  bool categorized() const;
  bool is_valid() const;
  bool is_public() const;

  void dump();

  bool has_params() const;
  bool has_return() const;

  bool has_param(const std::string& name);
  bool called_in_module() const;
  bool called_in_init() const;
  bool called_in_tick() const;
  bool called_in_tock() const;
  bool called_by_tock() const;
  bool called_in_func() const;

  //----------------------------------------

  MtModule* _mod = nullptr;
  MnNode _node;
  bool _public = false;
  MtModLibrary* _lib = nullptr;
  std::string _name;
  MnNode _type;
  TraceState state = CTX_NONE;

  //----------

  bool is_init_ = false;
  bool is_tick_ = false;
  bool is_tock_ = false;
  bool is_func_ = false;

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
