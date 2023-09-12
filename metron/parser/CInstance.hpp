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

struct CNode;
struct CNodeCall;
struct CNodeClass;
struct CNodeDeclaration;
struct CNodeField;
struct CNodeFunction;
struct CNodeStruct;
struct CNodeType;

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
  CInstance(CInstance* inst_parent, CNodeField* node_field);
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

  //----------

  virtual void dump_tree() const = 0;

  virtual CHECK_RETURN Err log_action(CNode* node, TraceAction action);

  virtual void commit_state() {
    assert(false);
  }

  //----------

  std::string_view get_name() const;
  CInstance* resolve(CNode* node);

  void push_state();
  void pop_state();
  void swap_state();
  void merge_state();

  CInstance* inst_parent = nullptr;
  std::map<std::string_view, CInstance*> inst_map;

  CNodeField* node_field = nullptr;
  std::vector<TraceState> state_stack;
};

//------------------------------------------------------------------------------

struct CInstClass : public CInstance {
  CInstClass(CInstance* inst_parent, CNodeField* node_field, CNodeClass* node_class);

  //----------

  void dump_tree() const override;

  CHECK_RETURN Err log_action(CNode* node, TraceAction action) override {
    Err err;
    for (auto pair : inst_map) err << pair.second->log_action(node, action);
    return err;
  }

  void commit_state() override {
    for (auto pair : inst_map) pair.second->commit_state();
  }

  //----------

  CNodeClass* node_class = nullptr;
  std::vector<CCall*> entry_points;
};

//------------------------------------------------------------------------------

struct CInstStruct : public CInstance {
  CInstStruct(CInstance* inst_parent, CNodeField* node_field, CNodeStruct* node_struct);

  //----------

  void dump_tree() const override;

  CHECK_RETURN Err log_action(CNode* node, TraceAction action) override {
    Err err;
    for (auto pair : inst_map) err << pair.second->log_action(node, action);
    return err;
  }

  void commit_state() override;

  //----------

  CNodeStruct* node_struct = nullptr;
};

//------------------------------------------------------------------------------

struct CInstPrim : public CInstance {
  CInstPrim(CInstance* inst_parent, CNodeField* node_field);

  //----------

  void dump_tree() const override;

  CHECK_RETURN Err log_action(CNode* node, TraceAction action) override {
    return CInstance::log_action(node, action);
  }

  void commit_state() override;

  //----------

};

//------------------------------------------------------------------------------

struct CCall {
  CCall(CInstClass* inst_class, CNodeCall* node_call, CNodeFunction* node_func);

  void dump_tree();

  CInstClass*    inst_class = nullptr;
  CNodeCall*     node_call  = nullptr;
  CNodeFunction* node_func  = nullptr;

  std::map<CNodeCall*, CCall*> call_map;
};

//------------------------------------------------------------------------------
