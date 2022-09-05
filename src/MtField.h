#pragma once

#include "MtNode.h"
#include "MtUtils.h"

struct MtModule;
struct MtMethod;
struct MtStruct;

//------------------------------------------------------------------------------

struct MtField {
  MtField(MtModule* _parent_mod, const MnNode& n, bool is_public);
  MtField(MtStruct* _parent_struct, const MnNode& n);

  const char* cname() const;
  const std::string& name() const;
  const std::string& type_name() const;

  bool is_component() const;
  bool is_struct() const;
  bool is_param() const;
  bool is_public() const;
  bool is_port() const {
    return is_public();
  }

  bool is_input() const { return _state == CTX_INPUT; }
  bool is_register() const {
    return _state == CTX_REGISTER || _state == CTX_MAYBE;
  }
  bool is_signal() const { return _state == CTX_OUTPUT || _state == CTX_SIGNAL; }
  bool is_dead() const { return _state == CTX_NONE; }
  bool is_public_input() const { return _public && is_input(); }
  bool is_public_signal() const { return _public && is_signal(); }
  bool is_public_register() const { return _public && is_register(); }
  bool is_private_signal() const { return !_public && is_signal(); }
  bool is_private_register() const { return !_public && is_register(); }

  MnNode get_type_node() const { return _node.get_field(field_type); }
  MnNode get_decl_node() const { return _node.get_field(field_declarator); }

  bool is_enum() { return _node.sym == sym_enum_specifier; }

  MtField* get_field(MnNode node);

  void dump() const;

  //----------

  MnNode _node;

  std::string _name;
  std::string _type;
  bool _public = false;
  bool _is_enum = false;

  MtModule* _parent_mod = nullptr;
  MtModule* _type_mod = nullptr;

  MtStruct* _parent_struct = nullptr;
  MtStruct* _type_struct = nullptr;

  ContextState _state = CTX_PENDING;
};

//------------------------------------------------------------------------------
