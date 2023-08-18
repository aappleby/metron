#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

#include <assert.h>
#include <vector>

//------------------------------------------------------------------------------

struct CNodeFunction : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0x0000FF; }

  virtual std::string_view get_name() const;

  bool is_constructor() const;
  bool is_public() const;
  bool is_tick() const { assert(false); return false; }
  bool is_tock() const { assert(false); return false; }

  bool has_params() const;
  bool has_return() const;

  bool called_in_module() const;
  bool called_in_init() const;
  bool called_in_tick() const;
  bool called_in_tock() const;
  bool called_in_func() const;

  void dump();

  virtual Err emit(Cursor& cursor);

  //----------------------------------------

  //MtModule* _mod = nullptr;
  //MnNode _node;
  //bool _public = false;
  //MtModLibrary* _lib = nullptr;
  //std::string _name;
  //MnNode _return_type;
  //TraceState state = CTX_NONE;

  //----------

  bool is_init_ = false;
  bool is_tick_ = false;
  bool is_tock_ = false;
  bool is_func_ = false;

  //bool emit_as_always_comb = false;
  //bool emit_as_always_ff = false;
  //bool emit_as_init = false;
  //bool emit_as_task = false;
  //bool emit_as_func = false;
  bool needs_binding = false;
  bool needs_ports = false;

  //std::vector<CNodeDeclaration*> param_nodes;
  //std::set<CNodeFunction*> internal_callers;
  //std::set<CNodeFunction*> internal_callees;
  //std::set<CNodeFunction*> external_callers;
  //std::set<CNodeFunction*> external_callees;

  // These are available after categorize_methods
  //std::set<CNodeFunction*> tick_callers;
  //std::set<CNodeFunction*> tock_callers;
  //std::set<CNodeFunction*> func_callers;

};

//------------------------------------------------------------------------------
