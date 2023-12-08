#pragma once

#include "metron/CNode.hpp"

#include <vector>
#include <set>
#include <functional>

struct CNode;
struct CNodeCompound;
struct CNodeDeclaration;
struct CNodeKeyword;
struct CNodeList;
struct CNodePunct;
struct CNodeType;

//------------------------------------------------------------------------------

struct CNodeFunction : public CNode {

  void init();

  bool should_emit_as_task();
  bool should_emit_as_func();

  bool called_by_init();
  bool has_return();

  using func_visitor = std::function<void(CNodeFunction*)>;

  void visit_internal_callees(func_visitor v);
  void visit_external_callees(func_visitor v);
  void visit_internal_callers(func_visitor v);
  void visit_external_callers(func_visitor v);

  void update_type();

  void set_type(MethodType new_type);
  bool needs_binding();

  bool must_call_before(CNodeFunction* func);

  //----------------------------------------

  MethodType method_type = MT_UNKNOWN;
  std::vector<CNodeDeclaration*> params;

  bool is_public = false;

  std::set<CInstance*> self_reads;
  std::set<CInstance*> self_writes;

  std::set<CNodeFunction*> internal_callers;
  std::set<CNodeFunction*> internal_callees;
  std::set<CNodeFunction*> external_callers;
  std::set<CNodeFunction*> external_callees;

  CNodeType*       node_type   = nullptr;
  CNode*           node_name   = nullptr;
  CNodeList*       node_params = nullptr;
  CNodeList*       node_init   = nullptr;
  CNodeKeyword*    node_const  = nullptr;
  CNodeCompound*   node_body   = nullptr;
  CNodePunct*      node_semi   = nullptr;
};

//------------------------------------------------------------------------------

struct CNodeConstructor : public CNodeFunction {};

//------------------------------------------------------------------------------
