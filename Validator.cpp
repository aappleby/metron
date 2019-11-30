#include "pch.h"
#include "Utils.h"

#if 0

//-----------------------------------------------------------------------------

class MetronRootVisitor : public RecursiveASTVisitor<MetronRootVisitor> {
public:

  typedef RecursiveASTVisitor<MetronRootVisitor> base;

  ASTContext* context;

  typedef set<NamedDecl*> DeclSet;
  typedef vector<DeclSet> DeclStack;

  DeclSet always;
  DeclSet maybe;

  vector<CXXMethodDecl*> callstack;

  bool inReset = false;
  bool inTick = false;
  bool inTock = false;

  set<CXXMethodDecl*> resetMethods;
  set<CXXMethodDecl*> tickMethods;
  set<CXXMethodDecl*> tockMethods;
  set<CXXMethodDecl*> otherMethods;

  map<CXXMethodDecl*, DeclSet> methodAlways;
  map<CXXMethodDecl*, DeclSet> methodMaybe;

  set<NamedDecl*> signals;
  set<NamedDecl*> states;

  enum Mode {
    IDLE,
    WRITE,
    READ
  };

  Mode mode = IDLE;

  //----------------------------------------

  MetronRootVisitor(ASTContext *Context) : base() {
    context = Context;
    reset();
  }

  void reset() {
    always.clear();
    maybe.clear();
  }

  //----------------------------------------

  void dumpDeclSet(const DeclSet& decls) {
    string out;
    sprintf(out, "{ ");
    for (const auto& d : decls) {
      sprintf(out, "%s ", d->getNameAsString().c_str());
    }
    sprintf(out, "}\n");
    dprintf(out.c_str());
  }

  //----------------------------------------
  // alwaysOut = alwaysA && alwaysB
  // maybeOut = (maybeA || maybeB || alwaysA || alwaysB) && !(alwaysA && alwaysB)

  static void foldParallel(const DeclSet& alwaysA, const DeclSet& maybeA,
                           const DeclSet& alwaysB, const DeclSet& maybeB,
                           DeclSet& alwaysOut, DeclSet& maybeOut) {
    for (const auto& d : alwaysA) {
      if (alwaysB.contains(d)) alwaysOut.insert(d);
      else maybeOut.insert(d);
    }

    for (const auto& d : alwaysB) {
      if (alwaysA.contains(d)) alwaysOut.insert(d);
      else maybeOut.insert(d);
    }

    maybeOut.insert(maybeA.begin(), maybeA.end());
    maybeOut.insert(maybeB.begin(), maybeB.end());
  }

  //----------
  // alwaysOut = alwaysA || alwaysB
  // maybeOut  = (maybeA || maybeB) && !(alwaysA || alwaysB);

  static void foldSeries(const DeclSet& alwaysA, const DeclSet& maybeA,
                         const DeclSet& alwaysB, const DeclSet& maybeB,
                         DeclSet& alwaysOut, DeclSet& maybeOut) {
    alwaysOut.insert(alwaysA.begin(), alwaysA.end());
    alwaysOut.insert(alwaysB.begin(), alwaysB.end());

    for (const auto& d : maybeA) {
      if (!alwaysOut.contains(d)) maybeOut.insert(d);
    }

    for (const auto& d : maybeB) {
      if (!alwaysOut.contains(d)) maybeOut.insert(d);
    }
  }

  //----------------------------------------

  bool VisitCXXMemberCallExpr(CXXMemberCallExpr* expr) {
    CXXMethodDecl* decl = expr->getMethodDecl();
    if (inTick) {
      dprintf("TICK CXXMemberCallExpr %s\n", decl->getNameAsString().c_str());
      if (tockMethods.contains(decl)) {
        dprintf("!!!!!! TOCK METHOD CALLED IN TICK\n");
      }
      else {
        tickMethods.insert(decl);
      }
    }
    if (inTock) {
      dprintf("TOCK CXXMemberCallExpr %s\n", decl->getNameAsString().c_str());
      if (tickMethods.contains(decl)) {
        dprintf("!!!!!! TICK METHOD CALLED IN TOCK\n");
      }
      else {
        tockMethods.insert(decl);
      }
    }
    return true;
  }

  /*
  bool VisitCXXMethodDecl(CXXMethodDecl* decl) {
    dprintf("method decl %s\n", decl->getNameAsString().c_str());
    return true;
  }
  */

  bool VisitMemberExpr(MemberExpr* e) {
    if (!e->isLValue()) return true;

    NamedDecl* d = e->getMemberDecl();
    string n = d->getNameAsString();

    if (mode == READ && inTick) {
      if (signals.contains(d)) {
        if (always.contains(d)) {
          dprintf("TICK CLEAN READ SIGNAL %s\n", n.c_str());
        } else {
          dprintf("TICK DIRTY READ SIGNAL %s\n", n.c_str());
        }
      }
      else if (states.contains(d)) {
        dprintf("TICK CLEAN READ STATE %s\n", n.c_str());
      }
      else {
        dprintf("TICK READ ????? %s\n", n.c_str());
      } 
    }
      
    if (mode == READ && inTock) {
      if (signals.contains(d)) {
        dprintf("TOCK CLEAN READ SIGNAL %s\n", n.c_str());
      }
      else if (states.contains(d)) {
        if (always.contains(d) || maybe.contains(d)) {
          dprintf("TOCK DIRTY READ STATE %s\n", n.c_str());
        } else {
          dprintf("TOCK CLEAN READ STATE %s\n", n.c_str());
        }
      }
      else {
        dprintf("TOCK READ ????? %s\n", n.c_str());
      }
    }
    
    if (mode == WRITE && inTick) {
      dprintf("TICK WRITE SIGNAL %s\n", n.c_str());
      always.insert(d);
      maybe.erase(d);
    }

    if (mode == WRITE && inTock) {
      dprintf("TOCK WRITE STATE %s\n", n.c_str());
      always.insert(d);
      maybe.erase(d);
    }

    return true;
  }

  //------------------------------

  bool TraverseIfStmt(IfStmt* S, DataRecursionQueue *Queue = nullptr) {
    dprintf("ifstmt\n");
    depth += 2;

    //dumpDeclSet(always);
    //dumpDeclSet(maybe);

    //----------

    dprintf("cond\n");
    depth += 2;
    TraverseStmt(S->getCond());
    DeclSet alwaysOld = always;
    DeclSet maybeOld = maybe;
    //dumpDeclSet(alwaysOld);
    //dumpDeclSet(maybeOld);
    depth -= 2;

    //----------

    dprintf("then\n");

    depth += 2;
    always.clear();
    maybe.clear();
    TraverseStmt(S->getThen());
    DeclSet alwaysA = always;
    DeclSet maybeA = maybe;
    //dumpDeclSet(alwaysA);
    //dumpDeclSet(maybeA);
    depth -= 2;

    //----------

    dprintf("else\n");

    depth += 2;
    always.clear();
    maybe.clear();
    TraverseStmt(S->getElse());
    DeclSet alwaysB = always;
    DeclSet maybeB = maybe;
    //dumpDeclSet(alwaysB);
    //dumpDeclSet(maybeB);
    depth -= 2;

    //----------

    DeclSet alwaysNew;
    DeclSet maybeNew;

    foldParallel(alwaysA, maybeA, alwaysB, maybeB, alwaysNew, maybeNew);

    //dumpDeclSet(alwaysNew);
    //dumpDeclSet(maybeNew);

    always.clear();
    maybe.clear();

    foldSeries(alwaysOld, maybeOld, alwaysNew, maybeNew, always, maybe);

    //dumpDeclSet(always);
    //dumpDeclSet(maybe);

    depth -= 2;
    return true;
  }

  //------------------------------
  // TraverseBinAddAssign
  // ...

  bool TraverseBinAddAssign(BinaryOperator *S, DataRecursionQueue *Queue = nullptr) {
    //dprintf("traverse binary add operator begin\n");
    //depth += 2;
    //bool result = base::TraverseBinAssign(S, Queue);

    mode = READ;
    TraverseStmt(S->getRHS());
    mode = WRITE;
    TraverseStmt(S->getLHS());
    mode = READ;

    //depth -= 2;
    //dprintf("traverse binary add operator end\n");
    return true;
  }

  bool TraverseBinAssign(BinaryOperator *S, DataRecursionQueue *Queue = nullptr) {
    //dprintf("traverse binary operator begin\n");
    //depth += 2;
    //bool result = base::TraverseBinAssign(S, Queue);

    mode = READ;
    TraverseStmt(S->getRHS());
    mode = WRITE;
    TraverseStmt(S->getLHS());
    mode = READ;
    
    //depth -= 2;
    //dprintf("traverse binary operator end\n");
    return true;
  }

  bool TraverseCXXMethodDecl(CXXMethodDecl* decl) {
    string n = decl->getNameAsString();

    callstack.push_back(decl);

    inTick = n.starts_with("tick");
    inTock = n.starts_with("tock") || n.starts_with("reset");

    if (inTick) tickMethods.insert(decl);
    if (inTock) tockMethods.insert(decl);

    dprintf("traverse method begin %s\n", decl->getNameAsString().c_str());
    depth += 2;
    bool result = base::TraverseCXXMethodDecl(decl);
    depth -= 2;
    dprintf("traverse method end\n");

    inTick = false;
    inTock = false;

    //dumpDeclSet(always);
    //dumpDeclSet(maybe);

    callstack.pop_back();

    methodAlways[decl] = always;
    methodMaybe[decl] = maybe;
    always.clear();
    maybe.clear();

    return result;
  }

  void scanRecord(CXXRecordDecl* decl) {
    dprintf("traverse record begin\n");
    depth += 2;
    base::TraverseCXXRecordDecl(decl);
    depth -= 2;
    dprintf("traverse record end\n");

    for (const auto& m : tickMethods) {
      dprintf("tick method %s\n", m->getNameAsString().c_str());
      const auto& ma = methodAlways[m];
      const auto& mm = methodMaybe[m];
      depth += 2;
      dumpDeclSet(ma);
      dumpDeclSet(mm);
      depth -= 2;
      signals.insert(ma.begin(), ma.end());
    }

    for (const auto& m : tockMethods) {
      dprintf("tock method %s\n", m->getNameAsString().c_str());
      const auto& ma = methodAlways[m];
      const auto& mm = methodMaybe[m];
      depth += 2;
      dumpDeclSet(ma);
      dumpDeclSet(mm);
      depth -= 2;
      states.insert(ma.begin(), ma.end());
      states.insert(mm.begin(), mm.end());
    }

    dprintf("signals\n");
    dumpDeclSet(signals);
    dprintf("states\n");
    dumpDeclSet(states);
  }

  //----------------------------------------

  bool TraverseCXXRecordDecl(CXXRecordDecl* decl) {
    /*
    dprintf("--------------- FIRST PASS ------------------");
    scanRecord(decl);

    dprintf("--------------- SECOND PASS ------------------");
    scanRecord(decl);
    */

    //MethodCollector c;
    //c.scanRecord(decl);

    return true;
  }

  //----------------------------------------

  bool scanTranslationUnit(TranslationUnitDecl* unit) {
    dprintf("TraverseTranslationUnitDecl\n");
    unit->dump();

    CallCollector c;
    c.scanTranslationUnit(unit);

    return true;
  }

};

#endif

//-----------------------------------------------------------------------------