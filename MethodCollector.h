#pragma once
#include "pch.h"

//-----------------------------------------------------------------------------
// Collects the sets of methods called by reset(), tick(), and tock().

class MethodCollector : public RecursiveASTVisitor<MethodCollector> {
public:
  typedef RecursiveASTVisitor<MethodCollector> base;

  bool inReset = false;
  bool inTick = false;
  bool inTock = false;

  FunctionSet resetRoots;
  FunctionSet tickRoots;
  FunctionSet tockRoots;

  FunctionSet resetMethods;
  FunctionSet tickMethods;
  FunctionSet tockMethods;

  bool isRoot  (CXXMethodDecl* d) { return resetRoots.contains(d)   || tickRoots.contains(d)   || tockRoots.contains(d); }
  bool isMethod(CXXMethodDecl* d) { return resetMethods.contains(d) || tickMethods.contains(d) || tockMethods.contains(d); }
  bool isReset (CXXMethodDecl* d) { return resetRoots.contains(d) || resetMethods.contains(d); }
  bool isTick  (CXXMethodDecl* d) { return tickRoots.contains(d)  || tickMethods.contains(d); }
  bool isTock  (CXXMethodDecl* d) { return tockRoots.contains(d)  || tockMethods.contains(d); }

  //----------------------------------------

  bool VisitCXXMemberCallExpr(CXXMemberCallExpr* expr);
  bool TraverseCXXMethodDecl(CXXMethodDecl* decl);
  void scanRecord(CXXRecordDecl* decl);
};

