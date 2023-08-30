#pragma once

#include <map>
#include <vector>
#include <string_view>

#include "metron/tools/MtUtils.h"

#include "metrolib/core/Log.h"
#include "metrolib/core/Platform.h"
#include "metrolib/core/Err.h"

#include <assert.h>

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
struct CInstClass;
struct CInstField;
struct CInstFunction;
struct CInstParam;
struct CInstStruct;

struct INamed;
struct IContext;
struct IMutable;
struct IDumpable;

//------------------------------------------------------------------------------

struct CLogEntry {
  TraceState  old_state;
  TraceState  new_state;
  TraceAction action;
  CNode*      cause;
};

//------------------------------------------------------------------------------
// Anything with a name.

struct INamed {
  virtual std::string_view get_name() const = 0;
};

//----------------------------------------
// Anything that can look up names. Classes, structs, functions, namespaces.

struct IContext {
  virtual INamed* resolve(std::string_view name) = 0;
};

//----------------------------------------
// Anything that can be read or written.
// Primitives, arrays, struct instances, call arguments, call returns.

struct IMutable {
  IMutable() { state_stack.push_back(CTX_NONE); }

  // Record an action applied to a mutable by the given parse node.
  virtual Err log_action(CNode* node, TraceAction action);

  std::vector<TraceState> state_stack;
  std::vector<CLogEntry>  action_log;
};

//----------------------------------------
// Anything that can be dumped. :D

struct IDumpable {
  virtual void dump_tree() = 0;
};

//------------------------------------------------------------------------------

/*
struct CInstTop : public IContext, IDumpable {
  CInstTop(CNodeTranslationUnit* node_unit);

  virtual INamed* resolve(std::string_view name);
  virtual void dump_tree();

  CNodeTranslationUnit* node_unit;
};
*/

//------------------------------------------------------------------------------

struct CInstClass : public INamed, IContext, IMutable, IDumpable {
  CInstClass(CNodeClass* node_class);

  virtual std::string_view get_name() const;
  virtual INamed* resolve(std::string_view name);
  virtual void dump_tree();

  CNodeClass* node_class;
  std::vector<CInstField*> inst_fields;
  std::vector<CInstFunction*>  entry_points;
};

//------------------------------------------------------------------------------
// Structs can be mutated directly "struct x; x = y;" and their fields can also
// be mutated individually.

struct CInstStruct : public INamed, IContext, IMutable, IDumpable {
  CInstStruct(CNodeStruct* node_struct);

  virtual std::string_view get_name() const;
  virtual INamed* resolve(std::string_view name);
  virtual void dump_tree();

  CNodeStruct* node_struct;
  std::vector<CInstField*> inst_fields;
};

//------------------------------------------------------------------------------

struct CInstField : public INamed, IDumpable {
  CInstField(CNodeField* node_field);

  virtual std::string_view get_name() const;
  virtual void dump_tree();

  CNodeField* node_field;
  IMutable*   inst_value;
};

//------------------------------------------------------------------------------

struct CInstReturn : public IMutable, IDumpable {
  CInstReturn(CNodeType* node_field);

  virtual void dump_tree();

  CNodeType*  node_type;
  IMutable*   inst_value;
};

//----------------------------------------

struct CInstArg : public INamed, IMutable, IDumpable {
  CInstArg(CNodeDeclaration* node_decl);

  virtual std::string_view get_name() const;
  virtual void dump_tree();

  IMutable*         inst_decl = nullptr;
  CNodeDeclaration* node_decl = nullptr; // The parameter declaration in the function param list
  CNodeExpression*  node_arg  = nullptr; // The expression at the corresponding slot in the callsite
};

//----------------------------------------

struct CInstFunction : public INamed, IContext, IDumpable {
  CInstFunction(IContext* parent, CNodeFunction* node_function);

  virtual std::string_view get_name() const;
  virtual INamed* resolve(std::string_view name);
  virtual void dump_tree();

  IContext* parent = nullptr;

  CNodeCall*     node_call;
  CNodeFunction* node_function;

  std::vector<CInstArg*>      inst_args;   // Function arguments
  std::vector<CInstFunction*> inst_calls;  // Call instances for everything this function calls.
  IMutable*                   inst_return; // Function return
};

//----------------------------------------
// IContext because we look up function arguments from it?

#if 0
struct CInstCall : public INamed, IContext, IDumpable {
  CInstCall(CNodeCall* node_call, CInstFunction* inst_func);

  virtual std::string_view get_name() const;
  virtual void dump_tree();

  // our parent is the CInstFunction containing this call, _not_ the function
  // we are _calling_.

  CNodeCall*     node_call = nullptr;
  CInstFunction* inst_func = nullptr; // <-- this is what we're calling
};
#endif

//------------------------------------------------------------------------------

struct CInstPrimitive : public IMutable, IDumpable {
  CInstPrimitive(CNodeType* node_type);

  virtual void dump_tree();

  CNodeType* node_type = nullptr;
};

//------------------------------------------------------------------------------

struct CInstArray : public IMutable, IDumpable {
  CInstArray(CNodeType* node_type, CNode* node_array);

  virtual void dump_tree();

  CNodeType* node_type = nullptr;
  CNode* node_array = nullptr;
};

//------------------------------------------------------------------------------
