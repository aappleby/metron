#include "pch.h"
#include "MethodCollector.h"
#include "Utils.h"

//----------------------------------------

bool MethodCollector::VisitCXXMemberCallExpr(CXXMemberCallExpr* expr) {
  CXXMethodDecl* decl = expr->getMethodDecl();
  std::string name = decl->getNameAsString();

  if (isRoot(decl)) {
    dprintf("!!!!! Root called inside another root!!!!!\n");
    return true;
  }

  if (!inReset && !inTick && !inTock) {
    dprintf("!!!!!!! CXXMethodDecl %s visited outside of reset/tick/tock\n", name.c_str());
    return true;
  }

  if (inReset) {
    dprintf("Calling %s during reset\n", name.c_str());
    resetMethods.insert(decl);
  }
  else if (inTick) {
    dprintf("Calling %s during tick\n", name.c_str());
    tickMethods.insert(decl);
  }
  else if (inTock) {
    dprintf("Calling %s during tock\n", name.c_str());
    tockMethods.insert(decl);
  }

  return true;
}

//----------------------------------------

bool MethodCollector::TraverseCXXMethodDecl(CXXMethodDecl* decl) {
  Indenter indent;
  string name = decl->getNameAsString();

  //dprintf("traverse method begin : %s\n", name.c_str());

  if (isRoot(decl)) {
    dprintf("!!!!! Already traversed this decl?\n");
    return true;
  }

  if (name.starts_with("reset")) {
    dprintf("New reset method %s\n", name.c_str());
    resetRoots.insert(decl);
    inReset = true;
  } else if (name.starts_with("tick")) {
    dprintf("New tick method %s\n", name.c_str());
    tickRoots.insert(decl);
    inTick = true;
  } else if (name.starts_with("tock")) {
    dprintf("New tock method %s\n", name.c_str());
    tockRoots.insert(decl);
    inTock = true;
  } else {
    dprintf("New other method %s\n", name.c_str());
  }

  if (inReset) resetMethods.insert(decl);
  if (inTick) tickMethods.insert(decl);
  if (inTock) tockMethods.insert(decl);

  bool result = base::TraverseCXXMethodDecl(decl);

  inReset = false;
  inTick = false;
  inTock = false;

  //dprintf("traverse method end : %s\n", name.c_str());

  return result;
}

//----------------------------------------

void MethodCollector::scanRecord(CXXRecordDecl* decl) {
  Indenter indent;

  dprintf("traverse record begin\n");
  base::TraverseCXXRecordDecl(decl);
  dprintf("traverse record end\n");

  dprintf("Reset roots:");
  for (auto d : resetRoots) dprintf(" %s", d->getNameAsString().c_str());
  dprintf("\n");

  dprintf("Tick roots:");
  for (auto d : tickRoots) dprintf(" %s", d->getNameAsString().c_str());
  dprintf("\n");

  dprintf("Tock roots:");
  for (auto d : tockRoots) dprintf(" %s", d->getNameAsString().c_str());
  dprintf("\n");

  dprintf("Reset methods :\n");
  for (auto d : resetMethods) dprintf(" %s", d->getNameAsString().c_str());
  dprintf("\n");

  dprintf("Tick methods :\n");
  for (auto d : tickMethods) dprintf(" %s", d->getNameAsString().c_str());
  dprintf("\n");

  dprintf("Tock methods :\n");
  for (auto d : tockMethods) dprintf(" %s", d->getNameAsString().c_str());
  dprintf("\n");
}

