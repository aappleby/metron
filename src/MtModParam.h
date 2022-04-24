#pragma once

#include "MtNode.h"

struct MtModule;

//------------------------------------------------------------------------------

struct MtModParam {
  MtModParam(MtModule* mod, MnNode& n);
  const std::string& name() const;
  const char* cname() const;
  const std::string& type_name() const;

  MnNode get_type_node() const;
  MnNode get_decl_node() const;

  void dump();

  MtModule* _mod;
  MnNode _node;
  std::string _name;
  std::string _type;
};

//------------------------------------------------------------------------------
