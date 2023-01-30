#pragma once
#include <vector>
#include <string>
#include <map>
#include "MtUtils.h"

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

//------------------------------------------------------------------------------

struct MtScope {
  MtScope();
  virtual ~MtScope();
  virtual void dump();
};

//------------------------------------------------------------------------------

struct MtInstance {
  MtInstance();
  virtual ~MtInstance();
  virtual const std::string& name() const;
  virtual void dump() {}

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


  LogEntry log_top;
  LogEntry log_next;
  std::vector<LogEntry> action_stack;
  std::vector<LogEntry> action_log;
};

//------------------------------------------------------------------------------

struct MtPrimitiveInstance : public MtInstance {
  MtPrimitiveInstance();
  virtual ~MtPrimitiveInstance();
  virtual const std::string& name() const;
  virtual void dump();
};

//------------------------------------------------------------------------------

struct MtArrayInstance : public MtInstance {
  MtArrayInstance();
  virtual ~MtArrayInstance();
  virtual const std::string& name() const;
  virtual void dump();
};

//------------------------------------------------------------------------------

struct MtStructInstance : public MtInstance {
  MtStructInstance(MtStruct* s);
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



  MtStruct* _struct;
  field_map _fields;
};

//------------------------------------------------------------------------------













//------------------------------------------------------------------------------

struct MtMethodInstance : public MtInstance {
  MtMethodInstance(MtModuleInstance* module, MtMethod* method);
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
};

//------------------------------------------------------------------------------

struct MtModuleInstance : public MtInstance {
  MtModuleInstance(MtModule* m);
  virtual ~MtModuleInstance();
  virtual void dump();

  MtMethodInstance* get_method(const std::string& name);
  MtInstance*       get_field (const std::string& name);

  virtual MtInstance* resolve(const std::vector<std::string>& path, int index);
  virtual void reset_state();

  MtModule*  _mod;
  field_map  _fields;
  method_map _methods;
};

//------------------------------------------------------------------------------
