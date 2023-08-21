#pragma once

#include <map>
#include <vector>

#include "metron/tools/MtUtils.h"

#include "metrolib/core/Log.h"

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
  virtual ~CInstance() {}
  virtual void dump();

  template <typename P>
  P* as_a() { return dynamic_cast<P*>(this); }

  template <typename P>
  P* is_a() { auto p = dynamic_cast<P*>(this); assert(p); return p; }

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
};

//------------------------------------------------------------------------------

struct CInstCall : public CInstance {
  CInstCall(CNodeCall* node_call);

  virtual void dump();

  CNodeCall* node_call;
  std::vector<CInstArg*> inst_args;
  CInstReturn* inst_return;
};

//----------------------------------------

struct CInstArg : public CInstance {
  CInstArg(CNodeExpression* node_arg);
  virtual void dump();
  CNodeExpression* node_arg;
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
