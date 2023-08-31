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
struct CInstCall;
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
  // Record an action applied to a mutable by the given parse node.
  virtual Err log_action(CNode* node, TraceAction action) = 0;
};

//----------------------------------------
// Anything that can be dumped. :D

struct IDumpable {
  virtual void dump_tree() = 0;
};

//------------------------------------------------------------------------------

struct CInstLog : public IMutable {
  CInstLog() { state_stack.push_back(CTX_NONE); }

  // Record an action applied to a mutable by the given parse node.
  virtual Err log_action(CNode* node, TraceAction action);

  std::vector<TraceState> state_stack;
  std::vector<CLogEntry>  action_log;
};

//------------------------------------------------------------------------------

struct CInstClass : public INamed, IContext, IMutable, IDumpable {
  CInstClass(CNodeClass* node_class);

  virtual std::string_view get_name() const;
  virtual INamed* resolve(std::string_view name);
  virtual Err log_action(CNode* node, TraceAction action);
  virtual void dump_tree();

  CNodeClass* node_class;
  std::vector<CInstField*>    inst_fields;
  std::vector<CInstCall*> entry_points;
};

//------------------------------------------------------------------------------
// Structs can be mutated directly "struct x; x = y;" and their fields can also
// be mutated individually.

struct CInstStruct : public INamed, IContext, IMutable, IDumpable {
  CInstStruct(CNodeStruct* node_struct);

  virtual std::string_view get_name() const;
  virtual INamed* resolve(std::string_view name);
  virtual Err log_action(CNode* node, TraceAction action);
  virtual void dump_tree();

  CNodeStruct* node_struct;
  std::vector<CInstField*> inst_fields;
};

//------------------------------------------------------------------------------

struct CInstField : public INamed, IMutable, IDumpable {
  CInstField(CNodeField* node_field);

  virtual std::string_view get_name() const;

  // Record an action applied to a mutable by the given parse node.
  virtual Err log_action(CNode* node, TraceAction action) {
    return inst_value->log_action(node, action);
  }

  virtual void dump_tree();


  CNodeField* node_field;
  IMutable*   inst_value;
};

//------------------------------------------------------------------------------

struct CInstReturn : public IMutable, IDumpable {
  CInstReturn(CNodeType* node_field);

  // Record an action applied to a mutable by the given parse node.
  virtual Err log_action(CNode* node, TraceAction action) {
    return inst_value->log_action(node, action);
  }

  virtual void dump_tree();

  CNodeType*  node_type;
  IMutable*   inst_value;
};

//----------------------------------------

struct CInstArg : public INamed, IMutable, IDumpable {
  CInstArg(CNodeDeclaration* node_decl);

  virtual std::string_view get_name() const;

  // Record an action applied to a mutable by the given parse node.
  virtual Err log_action(CNode* node, TraceAction action) {
    return inst_value->log_action(node, action);
  }

  virtual void dump_tree();

  CNodeDeclaration* node_param = nullptr; // The parameter declaration in the function param list
  IMutable*         inst_value = nullptr;
};

//----------------------------------------

struct CInstCall : public INamed, IContext, IDumpable {
  CInstCall(CInstClass* parent, CNodeCall* node_call);

  virtual std::string_view get_name() const;
  virtual INamed* resolve(std::string_view name);
  virtual void dump_tree();

  CInstClass* parent = nullptr;

  CNodeCall*     node_call;
  CNodeFunction* node_function;

  std::vector<CInstArg*>  inst_args;   // Function arguments
  IMutable*               inst_return = nullptr; // Function return
  std::vector<CInstCall*> inst_calls;  // Call instances for everything this function calls.
};

//------------------------------------------------------------------------------

struct CInstPrimitive : public CInstLog, IDumpable {
  CInstPrimitive(CNodeType* node_type);

  virtual void dump_tree();

  CNodeType* node_type = nullptr;
};

//------------------------------------------------------------------------------

struct CInstArray : public CInstLog, IDumpable {
  CInstArray(CNodeType* node_type, CNode* node_array);

  virtual void dump_tree();

  CNodeType* node_type = nullptr;
  CNode* node_array = nullptr;
};

//------------------------------------------------------------------------------
