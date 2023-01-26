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
  bool is_private() const;

  bool is_port() const;
  bool is_input() const;
  bool is_register() const;
  bool is_signal() const;
  bool is_dead() const;

  MnNode get_type_node() const;
  MnNode get_decl_node() const;

  bool is_enum();

  MtField* get_subfield(MnNode node);

  void dump() const;

  //----------

  MnNode _node;
  MnNode _type;
  MnNode _decl;

  ContextState _state = CTX_PENDING;

  // The module or structure that this field is a child of.
  MtModule* _parent_mod = nullptr;
  MtStruct* _parent_struct = nullptr;

  // The module or structure that this field is an instance of.
  MtModule* _type_mod = nullptr;
  MtStruct* _type_struct = nullptr;

private:

  std::string _name;
  std::string _type_name;

  bool _static = false;
  bool _const = false;
  bool _public = false;
  bool _enum = false;
};

//------------------------------------------------------------------------------
