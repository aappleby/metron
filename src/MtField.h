#pragma once

#include "MtNode.h"
#include "MtUtils.h"

//------------------------------------------------------------------------------

struct MtField {
  MtField(MtModule* _parent_mod, const MnNode& n, bool is_public);

  bool is_component() const;
  bool is_param() const;
  bool is_public() const;

  bool is_input_sig() const { return _public && (state == CTX_INPUT); }
  bool is_output_sig() const {
    return _public && (state == CTX_OUTPUT || state == CTX_SIGNAL);
  }
  bool is_output_reg() const { return _public && (state == CTX_REGISTER); }
  bool is_private_sig() const {
    return !_public && (state == CTX_OUTPUT || state == CTX_SIGNAL);
  }
  bool is_private_reg() const {
    return !_public && (state == CTX_INPUT || state == CTX_REGISTER);
  }

  const char* cname() const;
  const std::string& name() const;
  const std::string& type_name() const;

  MnNode get_type_node() const { return node.get_field(field_type); }
  MnNode get_decl_node() const { return node.get_field(field_declarator); }

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
