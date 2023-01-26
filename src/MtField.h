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

  bool is_port() const;
  bool is_input() const;
  bool is_register() const;
  bool is_signal() const;
  bool is_dead() const;
  bool is_public_input() const;
  bool is_public_signal() const;
  bool is_public_register() const;
  bool is_private_signal() const;
  bool is_private_register() const;

  MnNode get_type_node() const;
  MnNode get_decl_node() const;

  bool is_enum();

  MtField* get_field(MnNode node);

  void dump() const;

  //----------

  MnNode _node;

  bool _public = false;
  bool _is_enum = false;

  MtModule* _parent_mod = nullptr;
  MtModule* _type_mod = nullptr;

  MtStruct* _parent_struct = nullptr;
  MtStruct* _type_struct = nullptr;

  ContextState _state = CTX_PENDING;

private:
  std::string _name;
  std::string _type;

};

//------------------------------------------------------------------------------
