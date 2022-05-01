#pragma once

#include "MtNode.h"
#include "MtUtils.h"

//------------------------------------------------------------------------------

struct MtField {
  MtField(MtModule* _parent_mod, const MnNode& n, bool is_public);

  const char* cname() const;
  const std::string& name() const;
  const std::string& type_name() const;

  bool is_component() const;
  bool is_param() const;
  bool is_public() const;

  bool is_input() const { return state == CTX_INPUT; }
  bool is_register() const {
    return state == CTX_REGISTER || state == CTX_MAYBE;
  }
  bool is_signal() const { return state == CTX_OUTPUT || state == CTX_SIGNAL; }

  bool is_public_input() const { return _public && is_input(); }
  bool is_public_signal() const { return _public && is_signal(); }
  bool is_public_register() const { return _public && is_register(); }
  bool is_private_signal() const { return !_public && is_signal(); }
  bool is_private_register() const { return !_public && is_register(); }

  MnNode get_type_node() const { return node.get_field(field_type); }
  MnNode get_decl_node() const { return node.get_field(field_declarator); }

  bool is_enum() { return node.sym == sym_enum_specifier; }

  void dump() const;

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
