#pragma once

#include "MtField.h"

//------------------------------------------------------------------------------

struct MtContext {
  std::string get_path() const;
  MtContext* get_child(const std::string& name) const;
  void dump() const;

  MtContext* parent;
  std::string name;
  MtField* field;
  MtModule* mod;
  std::vector<MtContext*> children;
  FieldState state;
};

//------------------------------------------------------------------------------