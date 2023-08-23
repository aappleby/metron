#pragma once

#include <map>
#include <vector>

#include "metron/tools/MtUtils.h"

#include "metrolib/core/Log.h"
#include "metrolib/core/Platform.h"
#include "metrolib/core/Err.h"

struct CNode;
struct CNodeCall;
struct CNodeClass;
struct CNodeDeclaration;
struct CNodeExpression;
struct CNodeField;
struct CNodeFunction;
struct CNodeReturn;
struct CNodeStruct;
struct CNodeType;

struct CInstArg;
struct CInstCall;
struct CInstClass;
struct CInstField;
struct CInstFunction;
struct CInstParam;
struct CInstReturn;
struct CInstStruct;

//------------------------------------------------------------------------------

struct CLogEntry {
  TraceState  old_state;
  TraceState  new_state;
  TraceAction action;
  CNode*      cause;
};

//------------------------------------------------------------------------------

struct CInstance {
  CInstance() {
    state_stack.push_back(CTX_NONE);
  }
  virtual ~CInstance() {}
  virtual void dump();

  virtual CInstance* resolve(const std::string& name) {
    return nullptr;
  }

  template <typename P>
  P* as_a() { return dynamic_cast<P*>(this); }

  template <typename P>
  P* is_a() { auto p = dynamic_cast<P*>(this); assert(p); return p; }

  CHECK_RETURN Err log_action(CNode* node, TraceAction action) {
    Err err;
    auto old_state = state_stack.back();
    auto new_state = merge_action(old_state, action);

    //LOG_R("%s %s %s\n", to_string(action), to_string(old_state), to_string(new_state));

    if (old_state != new_state) {
      action_log.push_back({old_state, new_state, action, node});
    }

    state_stack.back() = new_state;

    if (new_state == CTX_INVALID) {
      LOG_R("Trace error: state went from %s to %s\n", to_string(old_state), to_string(new_state));
      dump();
      err << ERR("Invalid context state\n");
    }

    return err;
  }


  std::string _name;
  std::string _path;
  std::vector<TraceState> state_stack;
  std::vector<CLogEntry>   action_log;
};

//------------------------------------------------------------------------------

struct CInstClass : public CInstance {
  CInstClass(CNodeClass* node_class);
  virtual void dump();

  CNodeClass* node_class;
  std::vector<CInstField*> fields;
  std::vector<CInstFunction*> functions;
};

//------------------------------------------------------------------------------

struct CInstStruct : public CInstance {
  CInstStruct(CNodeStruct* node_struct);

  virtual void dump();

  CNodeStruct* node_struct;
  std::vector<CInstField*> inst_fields;
};

//------------------------------------------------------------------------------

struct CInstField : public CInstance {
  CInstField(CNodeField* node_field);
  virtual void dump();

  CNodeField* node_field;
  CInstance*  inst_decl;
};

//------------------------------------------------------------------------------

struct CInstFunction : public CInstance {
  CInstFunction(CNodeFunction* node_function);
  virtual void dump();

  CNodeFunction* node_function;
  std::vector<CInstParam*> inst_params;
  std::vector<CInstCall*>  inst_calls;
  CInstReturn* inst_return;
};

//----------------------------------------

struct CInstParam : public CInstance {
  CInstParam(CNodeDeclaration* node_decl);
  virtual void dump();

  CNodeDeclaration* node_decl;
  CInstance* inst_decl;
};

//----------------------------------------

struct CInstReturn : public CInstance {
  CInstReturn();

  CNodeType* node_return = nullptr;
};

//------------------------------------------------------------------------------

struct CInstCall : public CInstance {
  CInstCall();
  CInstCall(CNodeCall* node_call);

  virtual void dump();

  CNodeFunction* node_function = nullptr;
  CNodeCall* node_call = nullptr;
  std::vector<CInstArg*> inst_args;
  CInstReturn* inst_return = nullptr;
};

//----------------------------------------

struct CInstArg : public CInstance {
  CInstArg();
  CInstArg(CNodeExpression* node_arg);

  virtual void dump();
  CNode* node_param = nullptr;
  CNodeExpression* node_arg = nullptr;
};

//------------------------------------------------------------------------------

struct CInstPrimitive : public CInstance {
  CInstPrimitive(CNodeType* node_type);

  virtual void dump();
  CNodeType* node_type = nullptr;
};

struct CInstArray : public CInstance {
  CInstArray(CNodeType* node_type, CNode* node_array);

  virtual void dump();
  CNodeType* node_type = nullptr;
  CNode* node_array = nullptr;
};

//------------------------------------------------------------------------------
