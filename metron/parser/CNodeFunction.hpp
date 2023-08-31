#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

#include "metrolib/core/Log.h"

#include <assert.h>
#include <vector>
#include <set>

struct CNodeClass;

//------------------------------------------------------------------------------

struct CNodeParameter : public CNode {
  virtual uint32_t debug_color() const { return COL_YELLOW; }
};

//------------------------------------------------------------------------------

struct CNodeFunction : public CNode {

  //----------------------------------------
  // Methods to be implemented by subclasses.

  virtual uint32_t debug_color() const;
  virtual std::string_view get_name() const;
  virtual Err emit(Cursor& c);
  virtual Err trace(IContext* context);

  //----------------------------------------

  CNodeClass* get_parent_class();
  std::string_view get_return_type_name() const;

  void dump();


  //bool is_public() const;
  //bool is_tick() const { assert(false); return false; }
  //bool is_tock() const { assert(false); return false; }

  //bool has_params() const;
  //bool has_return() const;

  //bool called_in_module() const;
  //bool called_in_init() const;
  //bool called_in_tick() const;
  //bool called_in_tock() const;
  //bool called_in_func() const;

  //----------------------------------------

  //MtModule* _mod = nullptr;
  //MnNode _node;
  //bool _public = false;
  //MtModLibrary* _lib = nullptr;
  //std::string _name;
  //MnNode _return_type;
  //TraceState state = CTX_NONE;

  //----------

  bool is_public_ = false;
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
