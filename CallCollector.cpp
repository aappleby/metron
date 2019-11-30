#include "pch.h"
#include "CallCollector.h"
#include "Utils.h"

//-----------------------------------------------------------------------------

bool CallCollector::TraverseCXXMethodDecl(CXXMethodDecl* m) {
  dprintf("TraverseCXXMethodDecl %s\n", toString(m).c_str());
  currentFunction = m;
  base::TraverseCXXMethodDecl(m);
  currentFunction = nullptr;
  return true;
}

bool CallCollector::TraverseFunctionDecl(FunctionDecl* f) {
  dprintf("TraverseFunctionDecl %s\n", toString(f).c_str());
  currentFunction = f;
  base::TraverseFunctionDecl(f);
  currentFunction = nullptr;
  return true;
}

//----------

bool CallCollector::VisitCallExpr(CallExpr* c) {
  dprintf("VisitCallExpr %s\n", toString(c).c_str());
  callMap[currentFunction].insert(c->getDirectCallee());
  return true;
}

//-----------------------------------------------------------------------------
// Merges callee call maps with caller call maps until nothing changes.

void CallCollector::doTransitive() {
  std::vector<FunctionDecl*> temp;
  size_t delta = 1;

  while(delta != 0) {
    delta = 0;

    for (auto& pair : callMap) {
      auto& a_calls = pair.second;

      delta -= a_calls.size();

      temp.assign(a_calls.begin(), a_calls.end());
      for (auto& b : temp) {
        auto& b_calls = callMap[b];
        a_calls.insert(b_calls.begin(), b_calls.end());
      }

      delta += a_calls.size();
    }
  }
}

//-----------------------------------------------------------------------------
// Collects call lists for all methods in the record, then transitively merges them.

bool CallCollector::scanTranslationUnit(TranslationUnitDecl* unit) {
  TraverseTranslationUnitDecl(unit);
  dprintf("----------\n");
  dprintf("Call map:\n");
  for (const auto& pair : callMap) {
    if (pair.first) {
      dprintf("%s:", toString(pair.first).c_str());
    } else {
      dprintf("<???>:");
    }

    for (const auto& method : pair.second) {
      dprintf(" %s", toString(method).c_str());
    }
    dprintf("\n");
  }

  doTransitive();

  dprintf("----------\n");
  dprintf("Call map transitive:\n");
  for (const auto& pair : callMap) {
    if (pair.first) {
      dprintf("%s:", toString(pair.first).c_str());
    } else {
      dprintf("<???>:");
    }
    for (const auto& method : pair.second) {
      dprintf(" %s", toString(method).c_str());
    }
    dprintf("\n");
  }

  return true;
}

//-----------------------------------------------------------------------------