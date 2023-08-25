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
struct CNodeDeclaration;

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
  CInstance(CInstance* parent) : parent(parent) {
    state_stack.push_back(CTX_NONE);
  }
  virtual ~CInstance() {}
  virtual void dump_tree();

  virtual Err trace(TraceAction action) {
    LOG_R("unimp");
    exit(-1);
    return ERR("Can't trace CInstance base class\n");
  }

  virtual CInstance* resolve(const std::string& name) {
    LOG_R("unimp");
    exit(-1);
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
      dump_tree();
      err << ERR("Invalid context state\n");
    }

    return err;
  }

  CInstance* parent;
  std::string _name;
  std::string _path;
  std::vector<TraceState> state_stack;
  std::vector<CLogEntry>   action_log;
};

//------------------------------------------------------------------------------

struct CInstClass : public CInstance {
  CInstClass(CInstance* parent, CNodeClass* node_class);

  virtual CInstance* resolve(const std::string& name) {
    LOG_R("unimp");
    exit(-1);
    return nullptr;
  }

  virtual void dump_tree();

  CNodeClass* node_class;
  std::vector<CInstField*>    inst_fields;
  std::vector<CInstFunction*> inst_functions;
};

//------------------------------------------------------------------------------

struct CInstStruct : public CInstance {
  CInstStruct(CInstance* parent, CNodeStruct* node_struct);

  virtual CInstance* resolve(const std::string& name) {
    LOG_R("unimp");
    exit(-1);
    return nullptr;
  }

  virtual void dump_tree();

  CNodeStruct* node_struct;
  std::vector<CInstField*> inst_fields;
};

//------------------------------------------------------------------------------

struct CInstField : public CInstance {
  CInstField(CInstance* parent, CNodeField* node_field);

  virtual CInstance* resolve(const std::string& name) {
    LOG_R("unimp");
    exit(-1);
    return nullptr;
  }

  virtual void dump_tree();

  CNodeField* node_field;
  CInstance*  inst_decl;
};

//------------------------------------------------------------------------------

struct CInstFunction : public CInstance {
  CInstFunction(CInstance* parent, CNodeFunction* node_function);

  virtual CInstance* resolve(const std::string& name) {
    LOG_R("unimp");
    exit(-1);
    return nullptr;
  }

  virtual void dump_tree();

  CNodeFunction* node_function;
  std::vector<CInstParam*> inst_params;
  std::vector<CInstCall*>  inst_calls;
  CInstReturn* inst_return;
};

//----------------------------------------

struct CInstParam : public CInstance {
  CInstParam(CInstance* parent, CNodeDeclaration* node_decl);

  virtual CInstance* resolve(const std::string& name) {
    LOG_R("unimp");
    exit(-1);
    return nullptr;
  }

  virtual void dump_tree();

  CNodeDeclaration* node_decl;
  CInstance* inst_decl;
};

//----------------------------------------

struct CInstReturn : public CInstance {
  CInstReturn(CInstance* parent);

  virtual CInstance* resolve(const std::string& name) {
    LOG_R("unimp");
    exit(-1);
    return nullptr;
  }

  CNodeType* node_return = nullptr;
};

//------------------------------------------------------------------------------

struct CInstCall : public CInstance {
  CInstCall(CInstance* parent, CNodeCall* node_call, CInstFunction* inst_func);

  virtual CInstance* resolve(const std::string& name) {
    LOG_R("unimp");
    exit(-1);
    return nullptr;
  }

  virtual void dump_tree();

  CNodeCall*     node_call; // call node
  CInstFunction* inst_func; // target function instance

  CInstReturn*           inst_return; // our return value
  std::vector<CInstArg*> inst_args;   // our function arguments
};

//----------------------------------------

struct CInstArg : public CInstance {
  CInstArg(CInstance* parent, CNodeExpression* node_arg);

  virtual CInstance* resolve(const std::string& name) {
    LOG_R("unimp");
    exit(-1);
    return nullptr;
  }

  virtual void dump_tree();

  CNode* node_param = nullptr;
  CNodeExpression* node_arg = nullptr;
};

//------------------------------------------------------------------------------

struct CInstPrimitive : public CInstance {
  CInstPrimitive(CInstance* parent, CNodeType* node_type);

  virtual CInstance* resolve(const std::string& name) {
    LOG_R("unimp");
    exit(-1);
    return nullptr;
  }

  virtual void dump_tree();

  CNodeType* node_type = nullptr;
};

//------------------------------------------------------------------------------

struct CInstArray : public CInstance {
  CInstArray(CInstance* parent, CNodeType* node_type, CNode* node_array);

  virtual CInstance* resolve(const std::string& name) {
    LOG_R("unimp");
    exit(-1);
    return nullptr;
  }

  virtual void dump_tree();

  CNodeType* node_type = nullptr;
  CNode* node_array = nullptr;
};

//------------------------------------------------------------------------------
