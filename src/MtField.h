#pragma once

#include "MtNode.h"

//------------------------------------------------------------------------------

enum FieldState {
  FIELD_UNKNOWN = 0,
  FIELD_INPUT = 1,
  FIELD_OUTPUT = 2,
  FIELD_SIGNAL = 3,
  FIELD_REGISTER = 4,
  FIELD_INVALID = 5,
};

inline const char* to_string(FieldState f) {
  switch (f) {
    case FIELD_UNKNOWN:
      return "FIELD_UNKNOWN";
    case FIELD_INPUT:
      return "FIELD_INPUT";
    case FIELD_OUTPUT:
      return "FIELD_OUTPUT";
    case FIELD_SIGNAL:
      return "FIELD_SIGNAL";
    case FIELD_REGISTER:
      return "FIELD_REGISTER";
    default:
      return "FIELD_INVALID";
  }
}

//------------------------------------------------------------------------------

struct MtField {
  MtField(MtModule* _parent_mod, const MnNode& n, bool is_public);

  bool is_component() const;
  bool is_param() const;
  bool is_public() const;

  const char* cname() const;
  const std::string& name() const;
  const std::string& type_name() const;

  void dump();

  //----------

  FieldState state = FIELD_UNKNOWN;
  MnNode node;

  MtModule* _parent_mod;
  MtModule* _type_mod;
  std::string _name;
  std::string _type;
  bool _public = false;
};

//------------------------------------------------------------------------------
