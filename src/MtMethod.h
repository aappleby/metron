#pragma once

#include <set>
#include <string>
#include <vector>

#include "MtNode.h"

struct MtModule;
struct MtModLibrary;
struct MtField;

//------------------------------------------------------------------------------

struct MtMethod {
  MtMethod(MtModule* mod, MnNode n, bool is_public);

  const char* cname() const;
  std::string name() const;
  bool is_constructor() const;

  bool categorized() const;
  bool is_valid() const;
  bool is_root() const;
  bool is_leaf() const;
  bool is_branch() const;
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

  //----------------------------------------

  MtModule* _mod = nullptr;
  MnNode _node;
  bool _public = false;
  MtModLibrary* _lib = nullptr;
  std::string _name;
  MnNode _type;
  ContextState state = CTX_NONE;

  //----------

  bool in_init = false;
  bool in_tick = false;
  bool in_tock = false;
  bool in_func = false;
  bool is_toplevel = false;

  std::vector<MnNode> param_nodes;
  std::set<MtMethod*> internal_callers;
  std::set<MtMethod*> internal_callees;

  std::set<MtMethod*> external_callers;
  std::set<MtMethod*> external_callees;

  std::set<MtField*> writes;
};

//------------------------------------------------------------------------------
