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

  virtual void start_branch_a() {
    action_stack.push_back(log_next);
    log_next = log_top;
  }

  virtual void end_branch_a() {
  }

  virtual void start_branch_b() {
    std::swap(log_top, log_next);
  }

  virtual void end_branch_b() {
    log_top.state = merge_branch(log_top.state, log_next.state);
    log_next = action_stack.back();
    action_stack.pop_back();
  }

  virtual void start_switch() {
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

  virtual void start_case() {
    log_top = action_stack.back();
  }

  virtual void end_case() {
    log_next.state = merge_branch(log_top.state, log_next.state);
  }

  virtual void end_switch() {
    log_top = log_next;
    action_stack.pop_back();
    log_next = action_stack.back();
    action_stack.pop_back();
  }

  std::string path;
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

  void start_branch_a() {
    MtInstance::start_branch_a();
    //for (auto c : children) c->start_branch_a();
  }

  void end_branch_a() {
    MtInstance::end_branch_a();
    //for (auto c : children) c->end_branch_a();
  }

  void start_branch_b() {
    MtInstance::start_branch_b();
    //for (auto c : children) c->start_branch_b();
  }

  void end_branch_b() {
    MtInstance::end_branch_b();
    //for (auto c : children) c->end_branch_b();
  }

  void start_switch() {
    MtInstance::start_switch();
    //for (auto c : children) c->start_switch();
  }

  void start_case() {
    MtInstance::start_case();
    //for (auto c : children) c->start_case();
  }

  void end_case() {
    MtInstance::end_case();
    //for (auto c : children) c->end_case();
  }

  void end_switch() {
    MtInstance::end_switch();
    //for (auto c : children) c->end_switch();
  }

  virtual void visit(const inst_visitor& v) {
    v(this);
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

  std::string _name;
  MtMethod* _method;
  MtModuleInstance* _module;
  field_map _params;
  MtInstance* _retval = nullptr;

  bool has_local(const std::string& s) {
    for (int i = (int)scope_stack.size() - 1; i >= 0; i--) {
      if (scope_stack[i].contains(s)) return true;
    }
    return false;
  }

  std::vector<std::set<std::string>> scope_stack;
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
    v(this);
    for (auto& f : _fields)  v(f.second);
    for (auto& m : _methods) v(m.second);
  }

  MtModule*  _mod;
  field_map  _fields;
  method_map _methods;
};

//------------------------------------------------------------------------------
