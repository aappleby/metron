#pragma once

#include <map>
#include <vector>
#include <string_view>

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

  //----------------------------------------

  virtual std::string_view get_name() const;
  virtual Err trace(TraceAction action);
  virtual CInstance* resolve(std::string_view name);
  virtual void dump_tree();

  //----------------------------------------

  template <typename P>
  P* as_a() { return dynamic_cast<P*>(this); }

  //----------------------------------------

  CHECK_RETURN Err log_action(CNode* node, TraceAction action);

  //----------------------------------------

  CInstance* parent;
  std::vector<TraceState> state_stack;
  std::vector<CLogEntry>   action_log;
};

//------------------------------------------------------------------------------

struct CInstClass : public CInstance {
  CInstClass(CInstance* parent, CNodeClass* node_class);

  virtual std::string_view get_name() const;
  virtual Err trace(TraceAction action);
  virtual CInstance* resolve(std::string_view name);
  virtual void dump_tree();

  CNodeClass* node_class;
  std::vector<CInstField*>    inst_fields;
  std::vector<CInstFunction*> inst_functions;
};

//------------------------------------------------------------------------------

struct CInstStruct : public CInstance {
  CInstStruct(CInstance* parent, CNodeStruct* node_struct);

  virtual std::string_view get_name() const;
  virtual Err trace(TraceAction action);
  virtual CInstance* resolve(std::string_view name);
  virtual void dump_tree();

  CNodeStruct* node_struct;
  std::vector<CInstField*> inst_fields;
};

//------------------------------------------------------------------------------

struct CInstField : public CInstance {
  CInstField(CInstance* parent, CNodeField* node_field);

  virtual std::string_view get_name() const;
  virtual Err trace(TraceAction action);
  virtual CInstance* resolve(std::string_view name);
  virtual void dump_tree();

  CNodeField* node_field;
  CInstance*  inst_decl;
};

//------------------------------------------------------------------------------

struct CInstFunction : public CInstance {
  CInstFunction(CInstance* parent, CNodeFunction* node_function);

  virtual std::string_view get_name() const;
  virtual Err trace(TraceAction action);
  virtual CInstance* resolve(std::string_view name);
  virtual void dump_tree();

  CInstCall* get_call(CNodeCall* call);

  CNodeFunction* node_function;
  std::vector<CInstParam*> inst_params;
  std::vector<CInstCall*>  inst_calls;
  CInstReturn* inst_return;
};

//----------------------------------------

struct CInstParam : public CInstance {
  CInstParam(CInstance* parent, CNodeDeclaration* node_decl);

  virtual std::string_view get_name() const;
  virtual Err trace(TraceAction action);
  virtual CInstance* resolve(std::string_view name);
  virtual void dump_tree();


  CNodeDeclaration* node_decl;
  CInstance* inst_decl;
};

//----------------------------------------

struct CInstReturn : public CInstance {
  CInstReturn(CInstance* parent);

  virtual std::string_view get_name() const;
  virtual Err trace(TraceAction action);
  virtual CInstance* resolve(std::string_view name);
  virtual void dump_tree();

  CNodeType* node_return = nullptr;
};

//------------------------------------------------------------------------------

struct CInstCall : public CInstance {
  CInstCall(CInstance* parent, CNodeCall* node_call);

  virtual std::string_view get_name() const;
  virtual Err trace(TraceAction action);
  virtual CInstance* resolve(std::string_view name);
  virtual void dump_tree();

  // our parent is the CInstFunction containing this call
  CNodeCall*     node_call; // call node

  CInstReturn*           inst_return; // our return value
  std::vector<CInstArg*> inst_args;   // our function arguments
};

//----------------------------------------

struct CInstArg : public CInstance {
  CInstArg(CInstance* parent, CNodeExpression* node_arg);

  virtual std::string_view get_name() const;
  virtual Err trace(TraceAction action);
  virtual CInstance* resolve(std::string_view name);
  virtual void dump_tree();

  CNode* node_param = nullptr;
  CNodeExpression* node_arg = nullptr;
};

//------------------------------------------------------------------------------

struct CInstPrimitive : public CInstance {
  CInstPrimitive(CInstance* parent, CNodeType* node_type);

  virtual std::string_view get_name() const;
  virtual Err trace(TraceAction action);
  virtual CInstance* resolve(std::string_view name);
  virtual void dump_tree();


  CNodeType* node_type = nullptr;
};

//------------------------------------------------------------------------------

struct CInstArray : public CInstance {
  CInstArray(CInstance* parent, CNodeType* node_type, CNode* node_array);

  virtual std::string_view get_name() const;
  virtual Err trace(TraceAction action);
  virtual CInstance* resolve(std::string_view name);
  virtual void dump_tree();

  CNodeType* node_type = nullptr;
  CNode* node_array = nullptr;
};

//------------------------------------------------------------------------------
