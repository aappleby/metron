#include "pch.h"

#pragma warning(disable:4100)
#pragma warning(disable:4456)
#pragma warning(disable:4457)
#pragma warning(disable:4459)

using namespace tooling;
using namespace llvm;
using namespace ast_matchers;
using ast_type_traits::DynTypedNode;

typedef std::set<FunctionDecl*> FunctionSet;
typedef std::map<FunctionDecl*, FunctionSet> CallMap;

static cl::OptionCategory toolCategory("Metron options");
static cl::opt<string> opt_filename("o", cl::desc("Specify output filename"), cl::value_desc("filename"));

//-----------------------------------------------------------------------------

int depth = 0;
template<typename ... Args>
void dprintf(const char* format, Args ... args) {
  for(int i = 0; i < depth; i++) putchar(' ');
  printf(format, args...);
}

template<typename ... Args>
void sprintf(std::string& out, const char* format, Args ... args)
{
  char source_buf[1024];
  snprintf(source_buf, 1024, format, args ...);
  out.append(source_buf);
}

//-----------------------------------------------------------------------------
// Collects the set of methods directly and indirectly called by each method.

class CallCollector : public RecursiveASTVisitor<CallCollector> {
public:
  typedef RecursiveASTVisitor<CallCollector> base;

  FunctionDecl* currentFunction = nullptr;
  CallMap callMap;

  //----------------------------------------

  bool TraverseCXXMethodDecl(CXXMethodDecl* m) {
    dprintf("TraverseCXXMethodDecl %s\n", m->getNameAsString().c_str());
    currentFunction = m;
    base::TraverseCXXMethodDecl(m);
    currentFunction = nullptr;
    return true;
  }

  bool TraverseFunctionDecl(FunctionDecl* f) {
    dprintf("TraverseFunctionDecl %s\n", f->getNameAsString().c_str());
    currentFunction = f;
    base::TraverseFunctionDecl(f);
    currentFunction = nullptr;
    return true;
  }

  //----------

  bool VisitCallExpr(CallExpr* c) {
    dprintf("VisitCallExpr %s\n", c->getDirectCallee()->getNameAsString().c_str());
    callMap[currentFunction].insert(c->getDirectCallee());
    return true;
  }

  //----------------------------------------
  // Merges callee call maps with caller call maps until nothing changes.

  void doTransitive() {
    std::vector<FunctionDecl*> temp;
    size_t delta = 1;

    while(delta != 0) {
      //dprintf("x");
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

  //----------------------------------------
  // Collects call lists for all methods in the record, then transitively merges them.

  bool scanRecord(CXXRecordDecl* decl) {
    TraverseCXXRecordDecl(decl);
    dprintf("----------\n");
    dprintf("Call map:\n");
    depth += 2;
    for (const auto& pair : callMap) {
      if (pair.first) {
        dprintf("%s:", pair.first->getNameAsString().c_str());
      } else {
        dprintf("<???>:");
      }

      for (const auto& method : pair.second) {
        dprintf(" %s", method->getNameAsString().c_str());
      }
      dprintf("\n");
    }
    depth -= 2;

    doTransitive();

    dprintf("----------\n");
    dprintf("Call map transitive:\n");
    depth += 2;
    for (const auto& pair : callMap) {
      if (pair.first) {
        dprintf("%s:", pair.first->getNameAsString().c_str());
      } else {
        dprintf("<???>:");
      }
      for (const auto& method : pair.second) {
        dprintf(" %s", method->getNameAsString().c_str());
      }
      dprintf("\n");
    }
    depth -= 2;

    return true;
  }
};

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

  bool VisitCXXMemberCallExpr(CXXMemberCallExpr* expr) {
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

  bool TraverseCXXMethodDecl(CXXMethodDecl* decl) {
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

    depth += 2;
    bool result = base::TraverseCXXMethodDecl(decl);
    depth -= 2;

    inReset = false;
    inTick = false;
    inTock = false;

    //dprintf("traverse method end : %s\n", name.c_str());

    return result;
  }

  //----------------------------------------

  void scanRecord(CXXRecordDecl* decl) {
    dprintf("traverse record begin\n");
    depth += 2;
    base::TraverseCXXRecordDecl(decl);
    depth -= 2;
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
};

//-----------------------------------------------------------------------------

class MetronRootVisitor : public LexicallyOrderedRecursiveASTVisitor<MetronRootVisitor> {
public:

  typedef LexicallyOrderedRecursiveASTVisitor<MetronRootVisitor> base;

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

  MetronRootVisitor(ASTContext *Context) : base(Context->getSourceManager()) {
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

    CallCollector c;
    c.scanRecord(decl);

    return true;
  }

  //----------------------------------------

  bool scanTranslationUnit(TranslationUnitDecl* decl) {
    dprintf("TraverseTranslationUnitDecl\n");
    decl->dump();
    return base::TraverseTranslationUnitDecl(decl);
  }

};

//-----------------------------------------------------------------------------

/*
FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getBeginLoc());
if (FullLocation.isValid())
  llvm::outs() << "Found declaration at "
                << FullLocation.getSpellingLineNumber() << ":"
                << FullLocation.getSpellingColumnNumber() << "\n";
*/

class MetronASTConsumer : public clang::ASTConsumer {
public:
  MetronASTConsumer(ASTContext *Context) : Visitor(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.scanTranslationUnit(Context.getTranslationUnitDecl());
  }
private:
  MetronRootVisitor Visitor;
};

//----------

class MetronAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::unique_ptr<clang::ASTConsumer>(
        new MetronASTConsumer(&Compiler.getASTContext()));
  }
};

//----------

struct MetronFactory : public FrontendActionFactory {

  virtual std::unique_ptr<FrontendAction> clang::tooling::FrontendActionFactory::create(void) {
    return std::make_unique<MetronAction>();
  }
};

//-----------------------------------------------------------------------------

int main(int argc, const char** argv)
{
  CommonOptionsParser OptionsParser(argc, argv, toolCategory);
  ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
  Tool.run(new MetronFactory());

  return 0;
}

//-----------------------------------------------------------------------------