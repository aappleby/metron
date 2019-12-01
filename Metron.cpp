#include "pch.h"
#include "Utils.h"

#include "Visitor.h"

static llvm::cl::OptionCategory toolCategory("Metron options");

static llvm::cl::opt<string> opt_filename("o",
    llvm::cl::desc("Specify output filename"),
    llvm::cl::value_desc("filename"));

//-----------------------------------------------------------------------------
// alwaysOut = alwaysA && alwaysB
// maybeOut = (maybeA || maybeB || alwaysA || alwaysB) && !(alwaysA && alwaysB)

void foldParallel(const FieldSet& alwaysA, const FieldSet& maybeA,
                  const FieldSet& alwaysB, const FieldSet& maybeB,
                  FieldSet& alwaysOut, FieldSet& maybeOut) {
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

void foldSeries(const FieldSet& alwaysA, const FieldSet& maybeA,
                const FieldSet& alwaysB, const FieldSet& maybeB,
                FieldSet& alwaysOut, FieldSet& maybeOut) {
  alwaysOut.insert(alwaysA.begin(), alwaysA.end());
  alwaysOut.insert(alwaysB.begin(), alwaysB.end());

  for (const auto& d : maybeA) {
    if (!alwaysOut.contains(d)) maybeOut.insert(d);
  }

  for (const auto& d : maybeB) {
    if (!alwaysOut.contains(d)) maybeOut.insert(d);
  }
}

//-----------------------------------------------------------------------------

CXXMethodDecl* getMethodByPrefix(CXXRecordDecl* decl, const char* prefix) {
  for (auto m : decl->methods()) {
    if (m->getNameAsString().starts_with(prefix)) return m;
  }
  return nullptr;
}

//-----------------------------------------------------------------------------

void getReadsAndWrites(RecordDecl* record, Stmt* root, FieldSet& outReads, FieldSet& outWrites) {

  // First, put all MemberExprs in the body in the read set.
  set<MemberExpr*> reads;
  Visitor::visit(root, Stmt::MemberExprClass, [record, &reads](Stmt* stmt) {
    reads.insert(cast<MemberExpr>(stmt));
  });

  // Next, move all MemberExprs on the left hand side of an assignment from the read set to the write set.
  set<MemberExpr*> writes;
  Visitor::visit(root, Stmt::BinaryOperatorClass, [record, &reads, &writes](Stmt* stmt) {
    BinaryOperator* op = cast<BinaryOperator>(stmt);
    if (!op->isAssignmentOp()) return;

    MemberExpr* lhs = dyn_cast<MemberExpr>(op->getLHS());
    if (lhs == nullptr) return;

    reads.erase(lhs);
    writes.insert(lhs);
  });

  for (auto r : reads) {
    FieldDecl* field = dyn_cast<FieldDecl>(r->getMemberDecl());
    if (field == nullptr) return;
    if (field->getParent() != record) return;
    outReads.insert(field);
  }

  for (auto w : writes) {
    FieldDecl* field = dyn_cast<FieldDecl>(w->getMemberDecl());
    if (field == nullptr) return;
    if (field->getParent() != record) return;
    outWrites.insert(field);
  }
}

//-----------------------------------------------------------------------------

void getWriteExprs(CXXRecordDecl* record, Stmt* root, set<MemberExpr*>& out) {
  Visitor::visit(root, Stmt::BinaryOperatorClass, [&](Stmt* stmt) {
    BinaryOperator* op = cast<BinaryOperator>(stmt);
    if (!op->isAssignmentOp()) return;

    MemberExpr* lhs = dyn_cast<MemberExpr>(op->getLHS());
    if (lhs == nullptr) return;

    FieldDecl* field = dyn_cast<FieldDecl>(lhs->getMemberDecl());
    if (field == nullptr) return;
    if (field->getParent() != record) return;

    out.insert(lhs);
  });
}

//-----------------------------------------------------------------------------

bool validateReset(CXXRecordDecl* record, Stmt* root, FieldSet& signals, FieldSet& states, FieldSet& always, FieldSet& maybe) {
  bool result = true;

  set<MemberExpr*> writes;
  getWriteExprs(record, root, writes);

  Visitor v;
  v.stmtCallbackMap[Stmt::MemberExprClass] = [&](Stmt* stmt) {
    MemberExpr* expr = cast<MemberExpr>(stmt);
    FieldDecl* field = dyn_cast<FieldDecl>(expr->getMemberDecl());
    if (!field) {
      expr->dump();
      return;
    }

    bool isWrite   = writes.contains(expr);
    bool isSignal  = signals.contains(field);
    bool isState   = states.contains(field);
    bool isWritten = always.contains(field);

    if (isSignal) {
      dprintf("Signal %s is inaccessible during reset\n", toString(field).c_str());
      result = false;
    }

    if (isState && !isWrite && !isWritten) {
      dprintf("State %s is write-first during reset\n", toString(field).c_str());
      result = false;
    }

    if (!isSignal && !isState) {
      dprintf("Reference to unknown field %s in reset\n", toString(field).c_str());
      result = false;
    }

    if (isState && isWrite) {
      always.insert(field);
    }
  };

  v.stmtCallbackMap[Stmt::IfStmtClass] = [&](Stmt* stmt) {
    IfStmt* ifStmt = cast<IfStmt>(stmt);

    result &= validateReset(record, ifStmt->getCond(), signals, states, always, maybe);

    FieldSet alwaysA = always;
    FieldSet maybeA = maybe;
    result &= validateReset(record, ifStmt->getThen(), signals, states, alwaysA, maybeA);

    FieldSet alwaysB = always;
    FieldSet maybeB = maybe;
    result &= validateReset(record, ifStmt->getElse(), signals, states, alwaysB, maybeB);

    FieldSet alwaysNew;
    FieldSet maybeNew;
    foldParallel(alwaysA, maybeA, alwaysB, maybeB, alwaysNew, maybeNew);

    FieldSet alwaysOld = always;
    FieldSet maybeOld = maybe;
    always.clear();
    maybe.clear();

    foldSeries(alwaysOld, maybeOld, alwaysNew, maybeNew, always, maybe);
  };

  v.visit(root);
  return result;
}

//-----------------------------------------------------------------------------

bool validateTick(CXXRecordDecl* record, Stmt* root, FieldSet& signals, FieldSet& states, FieldSet& always, FieldSet& maybe) {
  bool result = true;

  set<MemberExpr*> writes;
  Visitor::visit(root, Stmt::BinaryOperatorClass, [record, &writes](Stmt* stmt) {
    BinaryOperator* op = cast<BinaryOperator>(stmt);
    if (!op->isAssignmentOp()) return;

    MemberExpr* lhs = dyn_cast<MemberExpr>(op->getLHS());
    if (lhs == nullptr) return;

    FieldDecl* field = dyn_cast<FieldDecl>(lhs->getMemberDecl());
    if (field == nullptr) return;
    if (field->getParent() != record) return;

    writes.insert(lhs);
  });

  Visitor v;
  v.stmtCallbackMap[Stmt::MemberExprClass] = [&](Stmt* stmt) {
    MemberExpr* expr = cast<MemberExpr>(stmt);
    FieldDecl* field = dyn_cast<FieldDecl>(expr->getMemberDecl());
    if (!field) {
      expr->dump();
      return;
    }

    bool isWrite   = writes.contains(expr);
    bool isSignal  = signals.contains(field);
    bool isState   = states.contains(field);
    bool isWritten = always.contains(field);

    if (isSignal && !isWrite && !isWritten) {
      dprintf("Signal %s is write-first during tick\n", toString(field).c_str());
      result = false;
    }

    if (isState && isWrite) {
      dprintf("State %s is read-only during tick\n", toString(field).c_str());
      result = false;
    }

    if (!isSignal && !isState) {
      dprintf("Reference to unkown field %s in tick\n", toString(field).c_str());
      result = false;
    }

    if (isSignal && isWrite) {
      always.insert(field);
    }
  };

  v.stmtCallbackMap[Stmt::IfStmtClass] = [&](Stmt* stmt) {
    IfStmt* ifStmt = cast<IfStmt>(stmt);

    result &= validateTick(record, ifStmt->getCond(), signals, states, always, maybe);

    FieldSet alwaysA = always;
    FieldSet maybeA = maybe;
    result &= validateTick(record, ifStmt->getThen(), signals, states, alwaysA, maybeA);

    FieldSet alwaysB = always;
    FieldSet maybeB = maybe;
    result &= validateTick(record, ifStmt->getElse(), signals, states, alwaysB, maybeB);

    FieldSet alwaysNew;
    FieldSet maybeNew;
    foldParallel(alwaysA, maybeA, alwaysB, maybeB, alwaysNew, maybeNew);

    FieldSet alwaysOld = always;
    FieldSet maybeOld = maybe;
    always.clear();
    maybe.clear();

    foldSeries(alwaysOld, maybeOld, alwaysNew, maybeNew, always, maybe);
  };

  v.visit(root);
  return result;
}

//-----------------------------------------------------------------------------

bool validateTock(CXXRecordDecl* record, Stmt* root, FieldSet& signals, FieldSet& states, FieldSet& always, FieldSet& maybe) {
  bool result = true;

  set<MemberExpr*> writes;
  getWriteExprs(record, root, writes);

  Visitor v;
  v.stmtCallbackMap[Stmt::MemberExprClass] = [&](Stmt* stmt) {
    MemberExpr* expr = cast<MemberExpr>(stmt);
    FieldDecl* field = dyn_cast<FieldDecl>(expr->getMemberDecl());
    if (!field) {
      expr->dump();
      return;
    }

    bool isWrite   = writes.contains(expr);
    bool isSignal  = signals.contains(field);
    bool isState   = states.contains(field);
    bool isWritten = always.contains(field) || maybe.contains(field);

    if (isState && !isWrite && isWritten) {
      dprintf("State %s is write-last during tock\n", toString(field).c_str());
      result = false;
    }

    if (isSignal && isWrite) {
      dprintf("Signal %s is read-only during tock\n", toString(field).c_str());
      result = false;
    }

    if (!isSignal && !isState) {
      dprintf("Reference to unknown field %s in tock\n", toString(field).c_str());
      result = false;
    }

    if (isState && isWrite) {
      always.insert(field);
    }
  };

  v.stmtCallbackMap[Stmt::IfStmtClass] = [&](Stmt* stmt) {
    IfStmt* ifStmt = cast<IfStmt>(stmt);

    result &= validateTock(record, ifStmt->getCond(), signals, states, always, maybe);

    FieldSet alwaysA = always;
    FieldSet maybeA = maybe;
    result &= validateTock(record, ifStmt->getThen(), signals, states, alwaysA, maybeA);

    FieldSet alwaysB = always;
    FieldSet maybeB = maybe;
    result &= validateTock(record, ifStmt->getElse(), signals, states, alwaysB, maybeB);

    FieldSet alwaysNew;
    FieldSet maybeNew;
    foldParallel(alwaysA, maybeA, alwaysB, maybeB, alwaysNew, maybeNew);

    FieldSet alwaysOld = always;
    FieldSet maybeOld = maybe;
    always.clear();
    maybe.clear();

    foldSeries(alwaysOld, maybeOld, alwaysNew, maybeNew, always, maybe);
  };

  v.visit(root);
  return result;
}

//-----------------------------------------------------------------------------

void getAlwaysMaybe(CXXRecordDecl* record, Stmt* root, FieldSet& always, FieldSet& maybe) {

  set<MemberExpr*> writes;
  getWriteExprs(record, root, writes);

  // Traverse the block, folding writes across if statement branches.
  Visitor v;

  v.stmtCallbackMap[Stmt::MemberExprClass] = [&](Stmt* stmt) {
    MemberExpr* expr = cast<MemberExpr>(stmt);
    if (writes.contains(expr)) {
      always.insert(cast<FieldDecl>(expr->getMemberDecl()));
    }
  };

  v.stmtCallbackMap[Stmt::IfStmtClass] = [&](Stmt* stmt) {
    IfStmt* ifStmt = cast<IfStmt>(stmt);

    getAlwaysMaybe(record, ifStmt->getCond(), always, maybe);

    FieldSet alwaysA = always;
    FieldSet maybeA = maybe;
    getAlwaysMaybe(record, ifStmt->getThen(), alwaysA, maybeA);

    FieldSet alwaysB = always;
    FieldSet maybeB = maybe;
    getAlwaysMaybe(record, ifStmt->getElse(), alwaysB, maybeB);

    FieldSet alwaysNew;
    FieldSet maybeNew;
    foldParallel(alwaysA, maybeA, alwaysB, maybeB, alwaysNew, maybeNew);

    FieldSet alwaysOld = always;
    FieldSet maybeOld = maybe;
    always.clear();
    maybe.clear();

    foldSeries(alwaysOld, maybeOld, alwaysNew, maybeNew, always, maybe);
  };

  v.visit(root);
}

//-----------------------------------------------------------------------------

bool isRecordValid(CXXRecordDecl* record) {
  if (!record) return false;
  record = record->getDefinition();
  if (!record) return false;

  bool hasReset = false;
  bool hasTick = false;
  bool hasTock = false;

  for (CXXMethodDecl* method : record->methods()) {
    if (!method->hasBody()) continue;
    hasReset |= method->getNameAsString().starts_with("reset");
    hasTick  |= method->getNameAsString().starts_with("tick");
    hasTock  |= method->getNameAsString().starts_with("tock");
  }

  return hasReset && hasTick && hasTock;
}

//-----------------------------------------------------------------------------
// Merges callee call maps with caller call maps until nothing changes.

void doTransitive(CallMap& callMap, CallMap& transitiveCallMap) {
  transitiveCallMap = callMap;
  vector<FunctionDecl*> temp;
  size_t delta = 1;

  while(delta != 0) {
    delta = 0;

    for (auto& pair : transitiveCallMap) {
      auto& a_calls = pair.second;

      delta -= a_calls.size();

      temp.assign(a_calls.begin(), a_calls.end());
      for (auto& b : temp) {
        auto& b_calls = transitiveCallMap[b];
        a_calls.insert(b_calls.begin(), b_calls.end());
      }

      delta += a_calls.size();
    }
  }
}

//-----------------------------------------------------------------------------

void getCalls(Stmt* root, FunctionSet& out) {
  auto callback = [&out](Stmt* s) {
    auto decl = cast<CallExpr>(s)->getDirectCallee()->getDefinition();
    if (decl) out.insert(decl);
  };

  Visitor v;
  v.stmtCallbackMap[Stmt::CXXMemberCallExprClass] = callback;
  v.stmtCallbackMap[Stmt::CallExprClass] = callback;
  v.visit(root);
}

//-----------------------------------------------------------------------------

bool validationPassed = true;

void process(ASTContext* context) {
  TranslationUnitDecl* unit = context->getTranslationUnitDecl();

  set<CXXRecordDecl*> records;

  Visitor::visit(unit, Decl::Kind::CXXRecord, [&](Decl* decl) {
    CXXRecordDecl* record = cast<CXXRecordDecl>(decl);
    if (!record->isThisDeclarationADefinition()) return;
    records.insert(record);
  });

  for (CXXRecordDecl* record : records) {
    CXXMethodDecl* reset = getMethodByPrefix(record, "reset");
    CXXMethodDecl* tick  = getMethodByPrefix(record, "tick");
    CXXMethodDecl* tock  = getMethodByPrefix(record, "tock");

    if (!reset || !tick || !tock) continue;

    FieldSet resetReads,resetWrites;
    FieldSet tickReads,signals;
    FieldSet tockReads,states;

    getReadsAndWrites(record, reset->getBody(), resetReads, resetWrites);
    getReadsAndWrites(record, tick->getBody(),  tickReads,  signals);
    getReadsAndWrites(record, tock->getBody(),  tockReads,  states);

    dprintf("----------\n");
    dprintf("Record %s\n", toString(record).c_str());
    indent();

    dprintf("Signals\n");
    indent();
    for (auto x : signals) dprintf("%s\n", toString(x).c_str());
    dedent();

    dprintf("States\n");
    indent();
    for (auto x : states) dprintf("%s\n", toString(x).c_str());
    dedent();

    dprintf("Reset %s\n", toString(reset).c_str());
    indent();
    for (auto x : resetReads)  dprintf("Read   %s\n", toString(x).c_str());
    for (auto x : resetWrites) dprintf("Write  %s\n", toString(x).c_str());
    dedent();

    dprintf("Tick %s\n", toString(tick).c_str());
    indent();
    for (auto x : tickReads) dprintf("Read   %s\n", toString(x).c_str());
    for (auto x : signals)   dprintf("Write  %s\n", toString(x).c_str());
    dedent();

    dprintf("Tock %s\n", toString(tock).c_str());
    indent();
    for (auto x : tockReads) dprintf("Read   %s\n", toString(x).c_str());
    for (auto x : states)    dprintf("Write  %s\n", toString(x).c_str());
    dedent();

    {
      dprintf("Validating reset\n");
      indent();
      FieldSet always;
      FieldSet maybe;
      validationPassed &= validateReset(record, reset->getBody(), signals, states, always, maybe);
      for (auto x : states) {
        if (!always.contains(x)) {
          dprintf("Reset: State %s was not always written\n", toString(x).c_str());
          validationPassed = false;
        }
      }
      dedent();
    }

    {
      dprintf("Validating tick\n");
      indent();
      FieldSet always;
      FieldSet maybe;
      validationPassed &= validateTick(record, tick->getBody(), signals, states, always, maybe);
      for (auto x : signals) {
        if (!always.contains(x)) {
          dprintf("Tick: Signal %s was not always written\n", toString(x).c_str());
          validationPassed = false;
        }
      }
      dedent();
    }

    {
      dprintf("Validating tock\n");
      indent();
      FieldSet always;
      FieldSet maybe;
      validationPassed &= validateTock(record, tock->getBody(), signals, states, always, maybe);
      dedent();
    }

    dedent();
  }
}

//-----------------------------------------------------------------------------

struct MetronASTConsumer : public ASTConsumer {
  virtual void HandleTranslationUnit(ASTContext& context) {
    process(&context);
  }
};

//----------

struct MetronAction : public ASTFrontendAction {
  virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(
    CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::unique_ptr<ASTConsumer>(new MetronASTConsumer());
  }
};

//----------

struct MetronFactory : public tooling::FrontendActionFactory {
  virtual std::unique_ptr<FrontendAction> tooling::FrontendActionFactory::create(void) {
    return std::make_unique<MetronAction>();
  }
};

//-----------------------------------------------------------------------------

int main(int argc, const char** argv)
{
  tooling::CommonOptionsParser OptionsParser(argc, argv, toolCategory);
  tooling::ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
  Tool.run(new MetronFactory());
  return validationPassed ? 0 : 1;
}

//-----------------------------------------------------------------------------






//-----------------------------------------------------------------------------

#if 0
 
vector<FieldDecl*> memberExprToPath(MemberExpr* expr) {
  vector<FieldDecl*> fields;

  while(expr) {
    FieldDecl* field = dyn_cast<FieldDecl>(expr->getMemberDecl());
    if (field) fields.push_back(field);

    Expr* base = expr->getBase();
    while(auto c = dyn_cast<CastExpr>(base)) base = c->getSubExpr();
    expr = dyn_cast<MemberExpr>(base);
  }

  std::reverse(fields.begin(), fields.end());
  return fields;
}

//----------

pair<int,int> fieldToRange(ASTContext* context, FieldDecl* field) {
  auto record = field->getParent();
  const auto& layout = context->getASTRecordLayout(record);

  int fieldIndex = field->getFieldIndex();
  int fieldCount = layout.getFieldCount();
  bool lastField = fieldIndex == fieldCount - 1;

  int fieldBegin = layout.getFieldOffset(fieldIndex);
  int fieldEnd = lastField ? int(layout.getDataSize().getQuantity()) * 8 : layout.getFieldOffset(fieldIndex + 1);

  return {fieldBegin, fieldEnd};
}

//----------

pair<int,int> pathToRange(ASTContext* context, vector<FieldDecl*> path) {
  pair<int,int> result = {0,0};

  for (size_t i = 0; i < path.size(); i++) {
    pair<int,int> range = fieldToRange(context, path[i]);
    result.first += range.first;
    result.second = range.second;
  }

  return result;
}

//----------

vector<FieldDecl*> rangeToPath(ASTContext* context, RecordDecl* record, pair<int,int> range) {
  vector<FieldDecl*> result;
 const auto& layout = context->getASTRecordLayout(record);
  int fieldCount = layout.getFieldCount();
  int recordSize = int(layout.getDataSize().getQuantity()) * 8;

  while(record) {
    for (int fieldIndex = 0; fieldIndex < fieldCount; fieldIndex++) {
      bool lastField = fieldIndex == fieldCount - 1;
      int fieldBegin = layout.getFieldOffset(fieldIndex);
      int fieldEnd = lastField ? recordSize : layout.getFieldOffset(fieldIndex + 1);
    }
  }
  return result;
}

#endif

//-----------------------------------------------------------------------------
