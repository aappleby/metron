#pragma once

#include "metron/tools/MtUtils.h"
#include "metrolib/core/Log.h"
#include "metrolib/core/Platform.h"
#include "metrolib/core/Err.h"

#include "NodeTypes.hpp"

#include <map>
#include <vector>
#include <string_view>
#include <assert.h>
#include <typeinfo>

struct CInstance;
struct CInstClass;
struct CInstStruct;
struct CInstPrim;

//------------------------------------------------------------------------------

struct CLogEntry {
  TraceState  old_state;
  TraceState  new_state;
  TraceAction action;
  CNode*      cause;
};

//------------------------------------------------------------------------------

struct CInstance {
  CInstance(std::string name, bool is_public, CInstance* inst_parent);
  virtual ~CInstance();

  template<typename T>
  T* as() {
    return dynamic_cast<T*>(this);
  }

  template<typename T>
  T* ancestor() {
    if (auto self = as<T>()) return self;
    if (inst_parent) return inst_parent->ancestor<T>();
    return nullptr;
  }


  CInstance* get_root() {
    return inst_parent ? inst_parent->get_root() : this;
  }

  std::string_view get_name() const;
  std::string get_path() const;
  CInstance* resolve(CNode* node);
  CInstance* resolve(std::string name);

  void push_state();
  void pop_state();
  void swap_state();
  void merge_state();

  virtual void dump_tree() const = 0;
  virtual CHECK_RETURN Err log_action(CNode* node, TraceAction action, call_stack& stack);
  virtual void commit_state() { assert(false); }
  bool check_port_directions(CInstance* b);

  //----------

  std::string name;
  bool is_public;
  bool is_constructor = false;
  CInstance* inst_parent = nullptr;
  std::vector<CInstance*> children;
  std::vector<TraceState> state_stack;
};

//------------------------------------------------------------------------------

CInstClass* instantiate_class(std::string name, bool is_public, CInstance* inst_parent, CNodeField* node_field, CNodeClass* node_class);

struct CInstClass : public CInstance {
  CInstClass(std::string name, bool is_public, CInstance* inst_parent, CNodeField* node_field, CNodeClass* node_class);

  void dump_tree() const override;
  CHECK_RETURN Err log_action(CNode* node, TraceAction action, call_stack& stack) override;
  void commit_state() override;

  //----------

  CNodeField* node_field = nullptr;
  CNodeClass* node_class = nullptr;
};

//------------------------------------------------------------------------------

struct CInstStruct : public CInstance {
  CInstStruct(std::string name, bool is_public, CInstance* inst_parent, CNodeField* node_field, CNodeStruct* node_struct);

  void dump_tree() const override;
  CHECK_RETURN Err log_action(CNode* node, TraceAction action, call_stack& stack) override;
  void commit_state() override;

  //----------

  CNodeField*  node_field = nullptr;
  CNodeStruct* node_struct = nullptr;
};

//------------------------------------------------------------------------------

struct CInstPrim : public CInstance {
  CInstPrim(std::string name, bool is_public, CInstance* inst_parent, CNodeField* node_field);

  void dump_tree() const override;
  CHECK_RETURN Err log_action(CNode* node, TraceAction action, call_stack& stack) override;
  void commit_state() override;

  CNodeField* node_field = nullptr;
};

//------------------------------------------------------------------------------

struct CInstFunc : public CInstance {
  CInstFunc(std::string name, bool is_public, CInstance* inst_parent, CNodeFunction* node_func);

  void commit_state() override {}
  void dump_tree() const override;
  CNodeFunction* node_func = nullptr;
};

//------------------------------------------------------------------------------
