#pragma once
#include "metrolib/core/Log.h"
#include "metron/nodes/MtNode.h"
#include "metron/tools/MtUtils.h"

#include <vector>
#include <string>
#include <map>
#include <set>
#include <functional>
#include <assert.h>

struct MtField;
struct MtMethod;
struct MtModule;
struct MnNode;
struct MtStruct;

struct MtInstance;
struct MtArrayInstance;
struct MtCallInstance;
struct MtMethodInstance;
struct MtModuleInstance;
struct MtPrimitiveInstance;
struct MtStructInstance;

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

  virtual CHECK_RETURN Err sanity_check();
  virtual CHECK_RETURN Err assign_types();
  virtual CHECK_RETURN Err log_action(MnNode node, TraceAction action);
  virtual void dump();
  virtual void dump_log();
  virtual void reset_state();
  virtual void visit(const inst_visitor& v);
  virtual MtInstance* resolve(const std::vector<std::string>& path, int index);
  virtual CHECK_RETURN Err merge_with_source();

  //----------

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
    //assert(state_stack.size() >= 2);
    if (state_stack.size() < 2) {
      dump();
      node.dump_tree();
      //assert(false);
      LOG_R("state_stack.size() >= 2\n");
      exit(1);
    }

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
  MtFieldInstance(const std::string& name, const std::string& path, MtField* field);
  virtual ~MtFieldInstance();

  // MtInstance
  virtual CHECK_RETURN Err sanity_check();
  virtual void reset_state();
  virtual CHECK_RETURN Err merge_with_source();

  //----------

  virtual FieldType get_field_type() const;
  virtual CHECK_RETURN Err set_field_type(FieldType f);

private:
  MtField*  _field;
  FieldType _field_type = FT_UNKNOWN;
};

//------------------------------------------------------------------------------

struct MtPrimitiveInstance : public MtFieldInstance {
  MtPrimitiveInstance(const std::string& name, const std::string& path, MtField* field);
  virtual ~MtPrimitiveInstance();

  // MtInstance
  virtual CHECK_RETURN Err sanity_check();
  virtual CHECK_RETURN Err assign_types();
  virtual void dump();
  virtual CHECK_RETURN Err merge_with_source();

  // MtFieldInstance
};

//------------------------------------------------------------------------------

struct MtArrayInstance : public MtFieldInstance {
  MtArrayInstance(const std::string& name, const std::string& path, MtField* field);
  virtual ~MtArrayInstance();

  // MtInstance
  virtual CHECK_RETURN Err sanity_check();
  virtual CHECK_RETURN Err assign_types();
  virtual void dump();
  virtual CHECK_RETURN Err merge_with_source();

  // MtFieldInstance
};

//------------------------------------------------------------------------------

struct MtStructInstance : public MtFieldInstance {
  MtStructInstance(const std::string& name, const std::string& path, MtStruct* s, MtField* field);
  virtual ~MtStructInstance();

  // MtInstance
  virtual CHECK_RETURN Err sanity_check();
  virtual CHECK_RETURN Err assign_types();
  virtual CHECK_RETURN Err log_action(MnNode node, TraceAction action);
  virtual void dump();
  virtual void reset_state();
  virtual void visit(const inst_visitor& v);
  virtual MtInstance* resolve(const std::vector<std::string>& path, int index);
  virtual CHECK_RETURN Err merge_with_source();

  // MtFieldInstance
  virtual FieldType get_field_type() const;
  virtual CHECK_RETURN Err set_field_type(FieldType f);

  //----------

  MtInstance* get_field(const std::string& name);

  MtStruct* _struct;
  std::vector<MtFieldInstance*> _fields;
};

//------------------------------------------------------------------------------

struct MtMethodInstance : public MtInstance {
  MtMethodInstance(const std::string& name, const std::string& path, MtModuleInstance* module, MtMethod* method);
  virtual ~MtMethodInstance();

  // MtInstance
  virtual CHECK_RETURN Err sanity_check();
  virtual CHECK_RETURN Err assign_types();
  virtual void dump();
  virtual void reset_state();
  virtual void visit(const inst_visitor& v);
  virtual CHECK_RETURN Err merge_with_source();

  //----------

  bool has_local(const std::string& s) {
    for (int i = (int)_scope_stack.size() - 1; i >= 0; i--) {
      if (_scope_stack[i].contains(s)) return true;
    }
    return false;
  }


  CHECK_RETURN Err set_method_type(MethodType t) {
    if(_method_type != MT_UNKNOWN && _method_type != t) {
      return ERR("MtMethodInstance::set_method_type - mismatch %s %s\n", to_string(_method_type), to_string(t));
    }
    _method_type = t;
    return Err::ok;
  }

  virtual MethodType get_method_type() const {
    return _method_type;
  }

  //----------

  MethodType _method_type = MT_UNKNOWN;
  MtMethod* _method;
  MtModuleInstance* _module;
  std::vector<std::set<std::string>> _scope_stack;
  std::set<MtInstance*> _writes;
  std::set<MtInstance*> _reads;
  std::set<MtMethodInstance*> _call_methods;
  std::set<MtMethodInstance*> _called_by;

  std::vector<MtCallInstance*> _calls;
};

//------------------------------------------------------------------------------

struct MtCallInstance : public MtInstance {
  MtCallInstance(
    const std::string& name,
    const std::string& path,
    MtCallInstance* caller,
    MnNode call_node,
    MtMethodInstance* method);
  virtual ~MtCallInstance();

  virtual void dump();
  virtual MtInstance* resolve(const std::vector<std::string>& path, int index);

  MtInstance* get_param(const std::string& name);

  MnNode _call_node;
  std::vector<MtInstance*> _params;
  MtInstance* _retval = nullptr;

  MtCallInstance* _caller= nullptr;
  std::vector<MtCallInstance*> _calls;

  MtModule* _module = nullptr;
  MtMethod* _method = nullptr;

  MtModuleInstance* _module_inst = nullptr;
  MtMethodInstance* _method_inst = nullptr;

  std::set<MtInstance*> _writes;
  std::set<MtInstance*> _reads;

  /*
  // MtInstance
  virtual CHECK_RETURN Err sanity_check();
  virtual CHECK_RETURN Err assign_types();
  virtual void reset_state();
  virtual void visit(const inst_visitor& v);
  virtual CHECK_RETURN Err merge_with_source();

  //----------

  MtInstance* get_param(const std::string& name);

  bool has_local(const std::string& s) {
    for (int i = (int)_scope_stack.size() - 1; i >= 0; i--) {
      if (_scope_stack[i].contains(s)) return true;
    }
    return false;
  }


  CHECK_RETURN Err set_method_type(MethodType t) {
    if(_method_type != MT_UNKNOWN && _method_type != t) {
      return ERR("MtMethodInstance::set_method_type - mismatch %s %s\n", to_string(_method_type), to_string(t));
    }
    _method_type = t;
    return Err::ok;
  }

  virtual MethodType get_method_type() const {
    return _method_type;
  }

  //----------

  MtMethodInstance* _method_inst = nullptr;

  std::vector<MtInstance*> _params;
  //MtInstance* _retval = nullptr;
  */
};

//------------------------------------------------------------------------------

struct MtModuleInstance : public MtFieldInstance {
  MtModuleInstance(const std::string& name, const std::string& path, MtModule* m, MtField* field);
  virtual ~MtModuleInstance();

  // MtInstance
  virtual CHECK_RETURN Err sanity_check();
  virtual CHECK_RETURN Err assign_types();
  virtual void dump();
  virtual void reset_state();
  virtual void visit(const inst_visitor& v);
  virtual MtInstance* resolve(const std::vector<std::string>& path, int index);
  virtual CHECK_RETURN Err merge_with_source();

  // MtFieldInstance
  virtual FieldType get_field_type() const;
  virtual CHECK_RETURN Err set_field_type(FieldType f);

  //----------

  MtMethodInstance* get_method(const std::string& name);
  MtInstance*       get_field (const std::string& name);

  //----------

  MtModule*  _mod;
  std::vector<MtInstance*> _fields;
  std::vector<MtMethodInstance*> _methods;
};

//------------------------------------------------------------------------------
