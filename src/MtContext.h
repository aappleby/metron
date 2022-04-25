#pragma once

#include "MtField.h"

//------------------------------------------------------------------------------

struct MtContext {
  MtContext() {}
  ~MtContext() {
    for (auto c : children) delete c;
    children.clear();
  }

  MtContext* clone();
  std::string get_path() const;
  MtContext* get_child(const std::string& name) const;

  static void instantiate(MtModule* mod, MtContext* inst);

  void assign_state_to_field();

  MtContext* resolve(const std::string& name);
  void dump() const;
  void dump_tree() const;

  MtMethod* current_method() {
    if (method) return method;
    if (parent) return parent->current_method();
    return nullptr;
  }

  ContextType type;
  MtContext* parent;
  std::string name;
  MtField* field;
  MtMethod* method;
  MtModule* mod;
  ContextState state;

  std::vector<MtContext*> children;

 private:
  MtContext(const MtContext& copy) = delete;
};

//------------------------------------------------------------------------------
