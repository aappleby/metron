#pragma once

#include "MtField.h"

//------------------------------------------------------------------------------

struct MtContext {
  MtContext(MtModule* top_mod);
  MtContext(MtContext* parent, MtMethod* _method);
  MtContext(MtContext* parent, MtField* _field);

  static MtContext* param(MtContext* parent, const std::string& name);
  static MtContext* construct_return(MtContext* parent);

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

  MtContext* parent;
  std::string name;
  ContextType type;
  ContextState state;

  MtField* field;
  MtMethod* method;
  MtModule* mod;

  std::vector<MtContext*> children;

 private:
  MtContext() {}
  MtContext(const MtContext& copy) = delete;
};

//------------------------------------------------------------------------------
