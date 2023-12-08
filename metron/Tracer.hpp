#pragma once

#include <assert.h>

#include <stack>
#include <vector>

#include "metrolib/core/Err.h"
#include "metron/MtUtils.h"
#include "metron/nodes/CNodeFunction.hpp"

struct CInstance;
struct CNode;
struct CNodeAccess;
struct CNodeAssignment;
struct CNodeBinaryExp;
struct CNodeBuiltinType;
struct CNodeCall;
struct CNodeCase;
struct CNodeClass;
struct CNodeClassType;
struct CNodeCompound;
struct CNodeConstant;
struct CNodeConstructor;
struct CNodeDeclaration;
struct CNodeDefault;
struct CNodeDoWhile;
struct CNodeEnum;
struct CNodeExpStatement;
struct CNodeField;
struct CNodeFieldExpression;
struct CNodeFor;
struct CNodeFunction;
struct CNodeIdentifier;
struct CNodeIdentifierExp;
struct CNodeIf;
struct CNodeKeyword;
struct CNodeList;
struct CNodeLValue;
struct CNodeNamespace;
struct CNodePrefixExp;
struct CNodePreproc;
struct CNodeQualifiedIdentifier;
struct CNodeReturn;
struct CNodeStruct;
struct CNodeSuffixExp;
struct CNodeSwitch;
struct CNodeTemplate;
struct CNodeTypedef;
struct CNodeUsing;
struct CSourceRepo;

//------------------------------------------------------------------------------

struct Tracer {

  void reset() {
    cstack.clear();
    istack.clear();
  }

  Err start_trace(CInstance* inst, CNodeFunction* func) {
    reset();
    cstack.push_back(func);
    istack.push_back(inst);
    return trace_dispatch(func);
  }

  Err trace_dispatch(CNode* node);
  Err trace_children(CNode* node);

  Err trace(CNodeAssignment* node);
  Err trace(CNodeCall* node);
  Err trace(CNodeFieldExpression* node);
  Err trace(CNodeFor* node);
  Err trace(CNodeIdentifier* node);
  Err trace(CNodeIdentifierExp* node);
  Err trace(CNodeIf* node);
  Err trace(CNodePrefixExp* node);
  Err trace(CNodeQualifiedIdentifier* node);
  Err trace(CNodeReturn* node);
  Err trace(CNodeSuffixExp* node);
  Err trace(CNodeSwitch* node);

  Err log_action(CInstance* inst, CNode* node, TraceAction action);
  Err log_action2(CInstance* inst, CNode* node, TraceAction action);

  std::vector<CNodeFunction*> cstack;
  std::vector<CInstance*> istack;

  bool in_constructor() {
    for (int i = cstack.size() - 1; i >= 0; i--) {
      if (cstack[i]->as<CNodeConstructor>()) return true;
    }
    return false;
  }

  CSourceRepo* repo = nullptr;
  CInstance* root_inst = nullptr;
  bool deep_trace = false;
  bool writes_are_bad = false;
  bool log_actions = false;
};
