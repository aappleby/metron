#pragma once

#include <assert.h>

#include <map>
#include <string_view>
#include <typeinfo>
#include <vector>

#include "metrolib/core/Err.h"
#include "metrolib/core/Log.h"
#include "metrolib/core/Platform.h"
#include "metron/CNode.hpp"
#include "metron/MtUtils.h"

struct CInstance;
struct CInstClass;
struct CInstStruct;
struct CInstPrim;
struct CInstance;
struct CNodeField;
struct CNodeClass;
struct CNodeStruct;
struct CNodeFunction;

// FIXME - we need a call stack that stores (inst,func) tuples instead of just
// func

CInstClass* instantiate_class(
  CSourceRepo* repo,
  std::string name,
  CInstance* inst_parent,
  CNodeField* node_field,
  CNodeClass* node_class,
  int depth);

//------------------------------------------------------------------------------

struct CInstance {
  CInstance(std::string name, CInstance* inst_parent);
  virtual ~CInstance();

  template <typename T>
  T* as() {
    return dynamic_cast<T*>(this);
  }

  template <typename T>
  T* ancestor() {
    if (auto self = as<T>()) return self;
    if (inst_parent) return inst_parent->ancestor<T>();
    return nullptr;
  }

  CInstance* get_root() { return inst_parent ? inst_parent->get_root() : this; }
  CInstance* resolve(CNode* node);

  //----------
  // CInstance interface

  virtual TraceState get_state() const = 0;
  virtual CInstance* resolve(std::string name) = 0;

  virtual void push_state() = 0;
  virtual void pop_state() = 0;
  virtual void swap_state() = 0;
  virtual void merge_state() = 0;

  //----------

  std::string name = "<invalid>";
  std::string path = "<invalid>";

  CInstance* inst_parent = nullptr;
};

//------------------------------------------------------------------------------

struct CInstClass : public CInstance {
  CInstClass(std::string name, CInstance* inst_parent, CNodeField* node_field,
             CNodeClass* node_class);

  bool check_port_directions(CInstClass* b);

  //----------
  // CInstance interface

  TraceState get_state() const override;
  CInstance* resolve(std::string name) override;

  void push_state() override;
  void pop_state() override;
  void swap_state() override;
  void merge_state() override;

  //----------

  std::vector<CInstance*> ports;
  std::vector<CInstance*> parts;

  CNodeField* node_field = nullptr;
  CNodeClass* node_class = nullptr;
};

//------------------------------------------------------------------------------

struct CInstStruct : public CInstance {
  CInstStruct(std::string name, CInstance* inst_parent, CNodeField* node_field,
              CNodeStruct* node_struct);

  //----------
  // CInstance interface

  TraceState get_state() const override;
  CInstance* resolve(std::string name) override;

  void push_state() override;
  void pop_state() override;
  void swap_state() override;
  void merge_state() override;

  //----------

  std::vector<CInstance*> parts;
  CNodeField* node_field = nullptr;
  CNodeStruct* node_struct = nullptr;
};

//------------------------------------------------------------------------------

struct CInstPrim : public CInstance {
  CInstPrim(std::string name, CInstance* inst_parent, CNodeField* node_field);

  //----------
  // CInstance interface

  TraceState get_state() const override;
  CInstance* resolve(std::string name) override;

  void push_state() override;
  void pop_state() override;
  void swap_state() override;
  void merge_state() override;

  //----------

  CNodeField* node_field = nullptr;
  std::vector<TraceState> state_stack;
};

//------------------------------------------------------------------------------

struct CInstFunc : public CInstance {
  CInstFunc(std::string name, CInstance* inst_parent, CNodeFunction* node_func);

  bool check_port_directions(CInstFunc* b);

  //----------
  // CInstance interface

  TraceState get_state() const override;
  CInstance* resolve(std::string name) override;

  void push_state() override;
  void pop_state() override;
  void swap_state() override;
  void merge_state() override;

  //----------

  std::vector<CInstance*> params;
  CInstance* inst_return = nullptr;

  CNodeFunction* node_func = nullptr;
};

//------------------------------------------------------------------------------
