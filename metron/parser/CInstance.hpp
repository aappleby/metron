#pragma once

#include <map>
#include <vector>
#include <string_view>

#include "metron/tools/MtUtils.h"

#include "metrolib/core/Log.h"
#include "metrolib/core/Platform.h"
#include "metrolib/core/Err.h"

#include <assert.h>
#include <typeinfo>

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

struct IContext;

//------------------------------------------------------------------------------

struct CLogEntry {
  TraceState  old_state;
  TraceState  new_state;
  TraceAction action;
  CNode*      cause;
};

//------------------------------------------------------------------------------

struct IContext {
  virtual ~IContext() {}

  virtual IContext* resolve(CNode* node) {
    LOG_R("bad bad %s\n", typeid(*this).name());
    assert(false);
    return nullptr;
  }

  virtual void dump_tree() {
    LOG_R("bad bad %s\n", typeid(*this).name());
    assert(false);
  }

  // Record an action applied to a mutable by the given parse node.
  virtual Err log_action(CNode* node, TraceAction action) {
    LOG_R("bad bad %s\n", typeid(*this).name());
    assert(false);
    return Err();
  }
};

//------------------------------------------------------------------------------

struct CInstLog : public IContext {
  CInstLog() { state_stack.push_back(CTX_NONE); }
  virtual ~CInstLog() {}

  // Record an action applied to a mutable by the given parse node.
  virtual Err log_action(CNode* node, TraceAction action) override;

  std::vector<TraceState> state_stack;
  std::vector<CLogEntry>  action_log;
};

//------------------------------------------------------------------------------

struct CInstClass : public IContext {
  CInstClass(CNodeClass* node_class);
  virtual ~CInstClass() {}

  std::string_view get_name() const;
  virtual IContext* resolve(CNode* node) override;
  virtual Err log_action(CNode* node, TraceAction action) override;
  virtual void dump_tree() override;

  CNodeClass* node_class = nullptr;
  std::vector<CInstField*> inst_fields;
  std::vector<CInstCall*>  entry_points;
};

//------------------------------------------------------------------------------
// Structs can be mutated directly "struct x; x = y;" and their fields can also
// be mutated individually.

struct CInstStruct : public IContext {
  CInstStruct(CNodeStruct* node_struct);
  virtual ~CInstStruct() {}

  std::string_view get_name() const;
  virtual IContext* resolve(CNode* node) override;
  virtual Err log_action(CNode* node, TraceAction action) override;
  virtual void dump_tree() override;

  CNodeStruct* node_struct = nullptr;
  std::vector<CInstField*> inst_fields;
};

//------------------------------------------------------------------------------

struct CInstField : public IContext {
  CInstField(CNodeField* node_field);

  std::string_view get_name() const;

  virtual IContext* resolve(CNode* node) override {
    return inst_value->resolve(node);
  }

  // Record an action applied to a mutable by the given parse node.
  virtual Err log_action(CNode* node, TraceAction action) override {
    return inst_value->log_action(node, action);
  }

  virtual void dump_tree() override;

  CNodeField* node_field = nullptr;
  IContext*   inst_value = nullptr;
};

//------------------------------------------------------------------------------

struct CInstReturn : public IContext {
  CInstReturn(CNodeType* node_field);

  std::string_view get_name() const {
    return "return";
  }

  // Record an action applied to a mutable by the given parse node.
  virtual Err log_action(CNode* node, TraceAction action) override {
    return inst_value->log_action(node, action);
  }

  virtual void dump_tree() override;

  CNodeType*  node_type = nullptr;
  IContext*   inst_value = nullptr;
};

//----------------------------------------

struct CInstArg : public IContext {
  CInstArg(CNodeDeclaration* node_decl);

  std::string_view get_name() const;

  // Record an action applied to a mutable by the given parse node.
  virtual Err log_action(CNode* node, TraceAction action) override {
    return inst_value->log_action(node, action);
  }

  virtual void dump_tree() override;

  CNodeDeclaration* node_param = nullptr; // The parameter declaration in the function param list
  IContext*         inst_value = nullptr;
};

//----------------------------------------

struct CInstCall : public IContext {
  CInstCall(CInstClass* parent, CNodeFunction* node_function, CNodeCall* node_call);

  std::string_view get_name() const;
  virtual IContext* resolve(CNode* node) override;
  virtual void dump_tree() override;

  CInstClass* parent = nullptr;

  CNodeCall*     node_call = nullptr;
  CNodeFunction* node_function = nullptr;

  std::vector<CInstArg*>  inst_args;   // Function arguments
  CInstReturn*            inst_return = nullptr; // Function return
  std::vector<CInstCall*> inst_calls;  // Call instances for everything this function calls.
};

//------------------------------------------------------------------------------

struct CInstPrimitive : public CInstLog {
  CInstPrimitive(CNodeType* node_type);

  virtual void dump_tree() override;

  CNodeType* node_type = nullptr;
};

//------------------------------------------------------------------------------

struct CInstArray : public CInstLog {
  CInstArray(CNodeType* node_type, CNode* node_array);

  virtual void dump_tree() override;

  CNodeType* node_type = nullptr;
  CNode* node_array = nullptr;
};

//------------------------------------------------------------------------------
