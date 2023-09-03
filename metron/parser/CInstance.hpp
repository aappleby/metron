#pragma once

#include "metron/tools/MtUtils.h"
#include "metrolib/core/Log.h"
#include "metrolib/core/Platform.h"
#include "metrolib/core/Err.h"
#include "CNode.hpp"

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
  T* as_a() {
    return dynamic_cast<T*>(this);
  }

  template<typename T>
  T* ancestor() {
    if (auto self = as_a<T>()) return self;
    if (inst_parent) return inst_parent->ancestor<T>();
    return nullptr;
  }

  virtual std::string_view get_name() const;
  virtual CInstance* resolve(CNode* node);
  virtual CHECK_RETURN Err log_action(CNode* node, TraceAction action);
  virtual void dump_tree() const;

  CInstance* inst_parent = nullptr;
  CNodeField* node_field = nullptr;
  std::vector<TraceState> state_stack;
};

//------------------------------------------------------------------------------

struct CInstClass : public CInstance {
  CInstClass(CInstance* inst_parent, CNodeField* node_field, CNodeClass* node_class);

  std::string_view get_name() const override;
  CInstance* resolve(CNode* name) override;
  CHECK_RETURN Err log_action(CNode* node, TraceAction action) override;
  void dump_tree() const override;

  CNodeClass* node_class = nullptr;

  std::map<std::string_view, CInstance*>   inst_map;

  /*
  std::map<std::string_view, CInstClass*>  class_map;
  std::map<std::string_view, CInstStruct*> struct_map;
  std::map<std::string_view, CInstPrim*>   prim_map;
  */

  std::vector<CCall*> entry_points;
};

//------------------------------------------------------------------------------

struct CInstStruct : public CInstance {
  CInstStruct(CInstance* inst_parent, CNodeField* node_field, CNodeStruct* node_struct);
  virtual ~CInstStruct() {}

  std::string_view get_name() const override;
  CInstance* resolve(CNode* node) override;
  CHECK_RETURN Err log_action(CNode* node, TraceAction action) override;
  void dump_tree() const override;

  CNodeStruct* node_struct = nullptr;
  std::map<std::string_view, CInstStruct*> struct_map;
  std::map<std::string_view, CInstPrim*>   prim_map;
};

//------------------------------------------------------------------------------

struct CInstPrim : public CInstance {
  CInstPrim(CInstance* inst_parent, CNodeField* node_field);

  CInstance* resolve(CNode* node) override;
  CHECK_RETURN Err log_action(CNode* node, TraceAction action) override;
  void dump_tree() const override;
};

//------------------------------------------------------------------------------

struct CCall {
  CCall(CInstClass* inst_class, CNodeCall* node_call, CNodeFunction* node_func);

  //CInstance* resolve(CNode* node);
  void dump_tree();

  CInstClass*    inst_class = nullptr;
  CNodeCall*     node_call  = nullptr;
  CNodeFunction* node_func  = nullptr;

  //std::map<std::string_view, CInstance*> inst_arg_map;
  //CInstance* inst_return;
  std::map<CNodeCall*, CCall*> inst_call_map;
};

//------------------------------------------------------------------------------
