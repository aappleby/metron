#pragma once

#include "MtNode.h"
#include "MtUtils.h"

//------------------------------------------------------------------------------

struct MtField {
  MtField(MtModule* _parent_mod, const MnNode& n, bool is_public);

  bool is_component() const;
  bool is_param() const;
  bool is_public() const;

  const char* cname() const;
  const std::string& name() const;
  const std::string& type_name() const;

  bool is_register() const { return state == CTX_REGISTER; }

  void dump();

  //----------

  ContextState state = CTX_PENDING;
  MnNode node;

  MtModule* _parent_mod;
  MtModule* _type_mod;
  std::string _name;
  std::string _type;
  bool _public = false;

  MtMethod* written_by = nullptr;
};

//------------------------------------------------------------------------------
