#pragma once

#include <set>
#include <string>
#include <vector>

#include "MtNode.h"

struct MtModule;
struct MtModLibrary;

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

  void dump();
  void dump_call_graph();

  //----------------------------------------

  MtModule* _mod = nullptr;
  MnNode _node;
  bool _public = false;
  MtModLibrary* _lib = nullptr;
  std::string _name;
  MnNode _type;
  bool has_params;
  bool has_return;
  ContextState state = CTX_PENDING;

  //----------

  bool in_init = false;
  bool in_tick = false;
  bool in_tock = false;
  bool in_func = false;

  std::vector<MnNode> param_nodes;
  std::set<MtMethod*> callers;
  std::set<MtMethod*> callees;
};

//------------------------------------------------------------------------------
