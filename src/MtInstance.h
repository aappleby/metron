#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include <functional>
#include <assert.h>
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

typedef std::function<void(MtInstance*)> inst_visitor;

//------------------------------------------------------------------------------

struct MtInstance {
  MtInstance(const std::string& name, const std::string& path);
  virtual ~MtInstance();
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
    log_top = merge_branch(log_top, log_next);
    log_next = action_stack.back();
    action_stack.pop_back();
  }

  void start_switch() {
    action_stack.push_back(log_next);
    action_stack.push_back(log_top);
    log_next = CTX_PENDING;
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
    log_next = merge_branch(log_top, log_next);
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


  struct LogEntry2 {
    TraceState state;
    //SourceRange range;
  };

  void push_state() {
    state_stack.push_back(state_stack.back());
  }

  void pop_state() {
    state_stack.pop_back();
  }

  void swap_state() {
    assert(state_stack.size() >= 2);
    std::swap(state_stack[state_stack.size() - 2], state_stack[state_stack.size() - 1]);
  }

  void merge_state() {
    assert(state_stack.size() >= 2);
    auto s = state_stack.size();
    state_stack[s-2] = merge_branch(state_stack[s-2], state_stack[s-1]);
    state_stack.pop_back();
  }

  std::string _name;
  std::string _path;
  FieldType  field_type = FT_UNKNOWN;
  TraceState log_top;
  TraceState log_next;
  std::vector<TraceState> action_stack;
  std::vector<TraceState> state_stack;
  std::vector<LogEntry2>  action_log;
};

//------------------------------------------------------------------------------

struct MtPrimitiveInstance : public MtInstance {
  MtPrimitiveInstance(const std::string& name, const std::string& path);
  virtual ~MtPrimitiveInstance();
  virtual void dump();
};

//------------------------------------------------------------------------------

struct MtArrayInstance : public MtInstance {
  MtArrayInstance(const std::string& name, const std::string& path);
  virtual ~MtArrayInstance();
  virtual void dump();
};

//------------------------------------------------------------------------------

struct MtStructInstance : public MtInstance {
  MtStructInstance(const std::string& name, const std::string& path, MtStruct* s);
  virtual ~MtStructInstance();
  virtual void dump();

  MtInstance* get_field(const std::string& name);
  virtual MtInstance* resolve(const std::vector<std::string>& path, int index);
  virtual void reset_state();

  virtual void visit(const inst_visitor& v) {
    MtInstance::visit(v);
    for (auto& f : _fields) v(f);
  }


  MtStruct* _struct;
  std::vector<MtInstance*> _fields;
};

//------------------------------------------------------------------------------

struct MtMethodInstance : public MtInstance {
  MtMethodInstance(const std::string& name, const std::string& path, MtModuleInstance* module, MtMethod* method);
  virtual ~MtMethodInstance();
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
      p->visit(v);
    }
  }

  MtMethod* _method;
  MtModuleInstance* _module;
  std::vector<MtInstance*> _params;
  MtInstance* _retval = nullptr;
  std::vector<std::set<std::string>> scope_stack;
  std::set<MtInstance*> writes;
  std::set<MtInstance*> reads;
};

//------------------------------------------------------------------------------

struct MtModuleInstance : public MtInstance {
  MtModuleInstance(const std::string& name, const std::string& path, MtModule* m);
  virtual ~MtModuleInstance();
  virtual void dump();

  MtMethodInstance* get_method(const std::string& name);
  MtInstance*       get_field (const std::string& name);

  virtual MtInstance* resolve(const std::vector<std::string>& path, int index);
  virtual void reset_state();

  virtual void visit(const inst_visitor& v) {
    MtInstance::visit(v);
    for (auto& f : _fields)  f->visit(v);
    for (auto& m : _methods) m->visit(v);
  }

  MtModule*  _mod;
  std::vector<MtInstance*> _fields;
  std::vector<MtMethodInstance*> _methods;
};

//------------------------------------------------------------------------------
