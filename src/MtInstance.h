#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include <functional>
#include <assert.h>
#include "MtUtils.h"
#include "Log.h"
#include "MtNode.h"

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

struct LogEntry {
  TraceState  old_state;
  TraceState  new_state;
  TraceAction action;
  MnNode node;
};

//------------------------------------------------------------------------------

struct MtInstance {
  MtInstance(const std::string& name, const std::string& path);
  virtual ~MtInstance();
  virtual void dump() {}
  virtual CHECK_RETURN Err sanity_check();
  virtual CHECK_RETURN Err assign_types();

  void dump_log();

  virtual void visit(const inst_visitor& v) { v(this); }

  virtual MtInstance* resolve(const std::vector<std::string>& path, int index) {
    return (index == path.size()) ? this : nullptr;
  }

  virtual void reset_state();

  void push_state(MnNode node) {
    state_stack.push_back(state_stack.back());
    action_log.push_back({state_stack.back(), state_stack.back(), ACT_PUSH, node});
  }

  void pop_state(MnNode node) {
    if (action_log.size() >= 2) {
      auto s = action_log.size();
      auto a = action_log[s-2];
      auto b = action_log[s-1];

      if (a.action == ACT_PUSH && b.action == ACT_SWAP) {
        action_log.pop_back();
        action_log.pop_back();
        state_stack.pop_back();
        return;
      }
    }

    action_log.push_back({state_stack.back(), state_stack.back(), ACT_POP, node});
    state_stack.pop_back();
  }

  void swap_state(MnNode node) {
    assert(state_stack.size() >= 2);

    auto s = state_stack.size();
    auto a = state_stack[s-2];
    auto b = state_stack[s-1];

    action_log.push_back({a, b, ACT_SWAP, node});

    state_stack[state_stack.size() - 2] = b;
    state_stack[state_stack.size() - 1] = a;
  }

  void merge_state(MnNode node) {
    if (action_log.size() >= 2) {
      auto s = action_log.size();
      auto a = action_log[s-2];
      auto b = action_log[s-1];

      if (a.action == ACT_PUSH && b.action == ACT_SWAP) {
        action_log.pop_back();
        action_log.pop_back();
        state_stack.pop_back();
        return;
      }
    }


    assert(state_stack.size() >= 2);
    auto s = state_stack.size();
    auto a = state_stack[s-2];
    auto b = state_stack[s-1];

    action_log.push_back({a, b, ACT_MERGE, node});

    state_stack[s-2] = merge_branch(a, b);
    state_stack.pop_back();
  }

  std::string _name;
  std::string _path;
  std::vector<TraceState> state_stack;
  std::vector<LogEntry>   action_log;
};

//------------------------------------------------------------------------------

struct MtFieldInstance : public MtInstance {
  MtFieldInstance(const std::string& name, const std::string& path) : MtInstance(name, path) {}
  FieldType _field_type = FT_UNKNOWN;
};

//------------------------------------------------------------------------------

struct MtPrimitiveInstance : public MtFieldInstance {
  MtPrimitiveInstance(const std::string& name, const std::string& path);
  virtual ~MtPrimitiveInstance();
  virtual void dump();
  virtual CHECK_RETURN Err sanity_check();
  CHECK_RETURN virtual Err assign_types();
};

//------------------------------------------------------------------------------

struct MtArrayInstance : public MtFieldInstance {
  MtArrayInstance(const std::string& name, const std::string& path);
  virtual ~MtArrayInstance();
  virtual void dump();
  virtual CHECK_RETURN Err sanity_check();
  CHECK_RETURN virtual Err assign_types();
};

//------------------------------------------------------------------------------

struct MtStructInstance : public MtFieldInstance {
  MtStructInstance(const std::string& name, const std::string& path, MtStruct* s);
  virtual ~MtStructInstance();
  virtual void dump();
  virtual CHECK_RETURN Err sanity_check();

  MtInstance* get_field(const std::string& name);
  virtual MtInstance* resolve(const std::vector<std::string>& path, int index);
  virtual void reset_state();

  virtual void visit(const inst_visitor& v) {
    MtInstance::visit(v);
    for (auto& f : _fields) v(f);
  }

  CHECK_RETURN virtual Err assign_types();

  MtStruct* _struct;
  std::vector<MtFieldInstance*> _fields;
};

//------------------------------------------------------------------------------

struct MtMethodInstance : public MtInstance {
  MtMethodInstance(const std::string& name, const std::string& path, MtModuleInstance* module, MtMethod* method);
  virtual ~MtMethodInstance();
  MtInstance* get_param(const std::string& name);
  virtual void dump();
  virtual CHECK_RETURN Err sanity_check();

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

  CHECK_RETURN virtual Err assign_types();

  MethodType _method_type = MT_UNKNOWN;
  MtMethod* _method;
  MtModuleInstance* _module;
  std::vector<MtInstance*> _params;
  MtInstance* _retval = nullptr;
  std::vector<std::set<std::string>> scope_stack;
  std::set<MtInstance*> writes;
  std::set<MtInstance*> reads;
  std::set<MtInstance*> calls;
  std::set<MtInstance*> called_by;
};

//------------------------------------------------------------------------------

struct MtModuleInstance : public MtFieldInstance {
  MtModuleInstance(const std::string& name, const std::string& path, MtModule* m);
  virtual ~MtModuleInstance();
  virtual void dump();
  virtual CHECK_RETURN Err sanity_check();

  MtMethodInstance* get_method(const std::string& name);
  MtInstance*       get_field (const std::string& name);

  virtual MtInstance* resolve(const std::vector<std::string>& path, int index);
  virtual void reset_state();

  virtual void visit(const inst_visitor& v) {
    MtInstance::visit(v);
    for (auto& f : _fields)  f->visit(v);
    for (auto& m : _methods) m->visit(v);
  }

  CHECK_RETURN virtual Err assign_types();

  MtModule*  _mod;
  std::vector<MtInstance*> _fields;
  std::vector<MtMethodInstance*> _methods;
};

//------------------------------------------------------------------------------
