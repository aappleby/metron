#pragma once

#include <string>

#include "MtNode.h"

//------------------------------------------------------------------------------

struct MtFuncParam {
  MtFuncParam(const std::string& func_name, const MnNode& n);
  const std::string& name() const;
  const char* cname() const;
  const std::string& type_name() const;

  MnNode get_type_node() const;
  MnNode get_decl_node() const;
  void dump();

  MnNode node;
  std::string func_name;
  std::string _name;
  std::string _type;
};

//------------------------------------------------------------------------------
