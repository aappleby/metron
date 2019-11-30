#pragma once
#include "pch.h"

typedef map<FunctionDecl*, FunctionSet> CallMap;

//-----------------------------------------------------------------------------
// Collects the set of methods directly and indirectly called by each method.

class CallCollector : public RecursiveASTVisitor<CallCollector> {
public:
  typedef RecursiveASTVisitor<CallCollector> base;

  FunctionDecl* currentFunction = nullptr;
  CallMap callMap;

  //----------------------------------------

  bool TraverseCXXMethodDecl(CXXMethodDecl* m);
  bool TraverseFunctionDecl(FunctionDecl* f);
  bool VisitCallExpr(CallExpr* c);
  void doTransitive();
  bool scanTranslationUnit(TranslationUnitDecl* unit);
};
