#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include <functional>
#include "MtUtils.h"
#include "Log.h"

struct MtField;
struct MtMethod;
struct MtModule;
struct MtStruct;
struct MnNode;


struct MtInstance;
struct MtArrayInstance;
struct MtPrimitiveInstance;
struct MtStructInstance;
struct MtMethodInstance;
struct MtModuleInstance;

typedef struct std::map<std::string, MtInstance*> field_map;
typedef struct std::map<std::string, MtMethodInstance*> method_map;

typedef std::function<void(MtInstance*)> inst_visitor;

//------------------------------------------------------------------------------

struct MtInstance {
  MtInstance(const std::string& path);
  virtual ~MtInstance();
  virtual const std::string& name() const;
  virtual void dump() {}

  virtual void visit(const inst_visitor& v) { v(this); }

  virtual MtInstance* resolve(const std::vector<std::string>& path, int index) {
    return (index == path.size()) ? this : nullptr;
  }

  virtual void reset_state();

  void start_branch_a() {
    action_stack.push_back(log_next);
    log_next = log_top;
  }

  void end_branch_a() {
  }

  void start_branch_b() {
    std::swap(log_top, log_next);
  }

  void end_branch_b() {
    log_top.state = merge_branch(log_top.state, log_next.state);
    log_next = action_stack.back();
    action_stack.pop_back();
  }

  void start_switch() {
    action_stack.push_back(log_next);
    action_stack.push_back(log_top);
    log_next.state = CTX_PENDING;
  }

  /*
    action_stack.push_back(log_next);
    action_stack.push_back(log_top);
    log_next.state = CTX_PENDING;

    log_top = action_stack.back();
    log_next.state = merge_branch(log_top.state, log_next.state);

    log_top = log_next;
    action_stack.pop_back();
    log_next = action_stack.back();
    action_stack.pop_back();

  */

  void start_case() {
    log_top = action_stack.back();
  }

  void end_case() {
    log_next.state = merge_branch(log_top.state, log_next.state);
  }

  void end_switch(bool has_default) {

    if (!has_default) {
      start_case();
      end_case();
    }

    log_top = log_next;
    action_stack.pop_back();
    log_next = action_stack.back();
    action_stack.pop_back();
  }

  std::string path;
  FieldType field_type = FT_UNKNOWN;
  LogEntry log_top;
  LogEntry log_next;
  std::vector<LogEntry> action_stack;
  std::vector<LogEntry> action_log;
};

//------------------------------------------------------------------------------

struct MtPrimitiveInstance : public MtInstance {
  MtPrimitiveInstance(const std::string& path);
  virtual ~MtPrimitiveInstance();
  virtual const std::string& name() const;
  virtual void dump();
};

//------------------------------------------------------------------------------

struct MtArrayInstance : public MtInstance {
  MtArrayInstance(const std::string& path);
  virtual ~MtArrayInstance();
  virtual const std::string& name() const;
  virtual void dump();
};

//------------------------------------------------------------------------------

struct MtStructInstance : public MtInstance {
  MtStructInstance(const std::string& path, MtStruct* s);
  virtual ~MtStructInstance();
  virtual const std::string& name() const;
  virtual void dump();

  MtInstance* get_field(const std::string& name);
  virtual MtInstance* resolve(const std::vector<std::string>& path, int index);
  virtual void reset_state();

  virtual void visit(const inst_visitor& v) {
    MtInstance::visit(v);
    for (auto& f : _fields) v(f.second);
  }


  MtStruct* _struct;
  field_map _fields;
};

//------------------------------------------------------------------------------

struct MtMethodInstance : public MtInstance {
  MtMethodInstance(const std::string& path, MtModuleInstance* module, MtMethod* method);
  virtual ~MtMethodInstance();
  virtual const std::string& name() const;
  MtInstance* get_param(const std::string& name);
  virtual void dump();

  virtual MtInstance* resolve(const std::vector<std::string>& path, int index);
  virtual void reset_state();

  bool has_local(const std::string& s) {
    for (int i = (int)scope_stack.size() - 1; i >= 0; i--) {
      if (scope_stack[i].contains(s)) return true;
    }
    return false;
  }

  virtual void visit(const inst_visitor& v) {
    MtInstance::visit(v);
    for (auto p : _params) {
      p.second->visit(v);
    }
  }

  std::string _name;
  MtMethod* _method;
  MtModuleInstance* _module;
  field_map _params;
  MtInstance* _retval = nullptr;
  std::vector<std::set<std::string>> scope_stack;
  std::set<MtInstance*> writes;
  std::set<MtInstance*> reads;
};

//------------------------------------------------------------------------------

struct MtModuleInstance : public MtInstance {
  MtModuleInstance(const std::string& path, MtModule* m);
  virtual ~MtModuleInstance();
  virtual void dump();

  MtMethodInstance* get_method(const std::string& name);
  MtInstance*       get_field (const std::string& name);

  virtual MtInstance* resolve(const std::vector<std::string>& path, int index);
  virtual void reset_state();

  virtual void visit(const inst_visitor& v) {
    MtInstance::visit(v);
    for (auto& f : _fields)  f.second->visit(v);
    for (auto& m : _methods) m.second->visit(v);
  }

  MtModule*  _mod;
  field_map  _fields;
  method_map _methods;
};

//------------------------------------------------------------------------------
