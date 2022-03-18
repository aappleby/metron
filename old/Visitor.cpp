#include "pch.h"
#include "Visitor.h"
#include "Utils.h"

//-----------------------------------------------------------------------------

void Visitor::visit(Decl* decl) {
  if (decl == nullptr) return;

  if (declCallbackMap.contains(decl->getKind())) {
    declCallbackMap[decl->getKind()](decl);
    return;
  }

  switch(decl->getKind()) {
  case Decl::Kind::TranslationUnit:  return visitUnit(cast<TranslationUnitDecl>(decl));
  case Decl::Kind::CXXRecord:        return visitCXXRecord(cast<CXXRecordDecl>(decl));
  case Decl::Kind::Record:           return visitRecord(cast<RecordDecl>(decl));
  case Decl::Kind::CXXMethod:        return visitMethod(cast<CXXMethodDecl>(decl));
  case Decl::Kind::Function:         return visitFunction(cast<FunctionDecl>(decl));
  case Decl::Kind::Var:              return visitVar(cast<VarDecl>(decl));
  case Decl::Kind::ParmVar:          return visitParmVar(cast<ParmVarDecl>(decl));
  case Decl::Kind::Field:            return visitField(cast<FieldDecl>(decl));
  case Decl::Kind::Typedef:          return visitTypedef(cast<TypedefDecl>(decl));
  case Decl::Kind::CXXConversion:    return visitConversion(cast<CXXConversionDecl>(decl));
  case Decl::Kind::FunctionTemplate: return; // FIXME
  case Decl::Kind::LinkageSpec:      return; // FIXME
  case Decl::Kind::PragmaDetectMismatch: return; // FIXME
  }

  dprintf("???visitDecl(%s)\n", decl->getDeclKindName());
  //decl->dump();
}

void Visitor::visitChildDecls(Decl* decl) {
  if (auto context = dyn_cast<DeclContext>(decl)) {
    for (auto child : context->decls()) visit(child);
  }
}

void Visitor::visitChildStmts(Decl* decl) {
  if (decl == nullptr) return;

  switch(decl->getKind()) {
  case Decl::Kind::TranslationUnit: return;
  case Decl::Kind::CXXRecord:       return;
  case Decl::Kind::Record:          return;
  case Decl::Kind::CXXMethod:       return visit(cast<CXXMethodDecl>(decl)->getBody());
  case Decl::Kind::Function:        return visit(cast<FunctionDecl>(decl)->getBody());
  case Decl::Kind::Var:             return visit(cast<VarDecl>(decl)->getInit());
  case Decl::Kind::ParmVar:         return visit(cast<ParmVarDecl>(decl)->getDefaultArg());
  case Decl::Kind::Field:           return visit(cast<FieldDecl>(decl)->getInClassInitializer());
  case Decl::Kind::Typedef:         return;
  case Decl::Kind::CXXConversion:   return visit(cast<CXXConversionDecl>(decl)->getBody());
  }

  dprintf("???visitDeclChildStmts(%s)\n", decl->getDeclKindName());
  //decl->dump();
}

void Visitor::visitUnit(TranslationUnitDecl* decl)     { visitOtherDecl(decl); }
void Visitor::visitCXXRecord(CXXRecordDecl* decl)      { visitOtherDecl(decl); }
void Visitor::visitRecord(RecordDecl* decl)            { visitOtherDecl(decl); }
void Visitor::visitMethod(CXXMethodDecl* decl)         { visitOtherDecl(decl); }
void Visitor::visitFunction(FunctionDecl* decl)        { visitOtherDecl(decl); }
void Visitor::visitVar(VarDecl* decl)                  { visitOtherDecl(decl); }
void Visitor::visitParmVar(ParmVarDecl* decl)          { visitOtherDecl(decl); }
void Visitor::visitField(FieldDecl* decl)              { visitOtherDecl(decl); }
void Visitor::visitTypedef(TypedefDecl* decl)          { visitOtherDecl(decl); }
void Visitor::visitConversion(CXXConversionDecl* decl) { visitOtherDecl(decl); }

void Visitor::visitOtherDecl(Decl* decl) {
  visitChildDecls(decl);
  visitChildStmts(decl);
}

//-----------------------------------------------------------------------------

void Visitor::visit(Stmt* stmt) {
  if (stmt == nullptr) return;

  if (stmtCallbackMap.contains(stmt->getStmtClass())) {
    stmtCallbackMap[stmt->getStmtClass()](stmt);
    return;
  }

  switch(stmt->getStmtClass()) {
  case Stmt::CallExprClass:            return visitCall(cast<CallExpr>(stmt));
  case Stmt::CompoundStmtClass:        return visitCompound(cast<CompoundStmt>(stmt));
  case Stmt::CXXThisExprClass:         return visitThis(cast<CXXThisExpr>(stmt));
  case Stmt::DeclStmtClass:            return visitDeclStmt(cast<DeclStmt>(stmt));
  case Stmt::DeclRefExprClass:         return visitDeclRef(cast<DeclRefExpr>(stmt));
  case Stmt::BinaryOperatorClass:      return visitBinOp(cast<BinaryOperator>(stmt));
  case Stmt::MemberExprClass:          return visitMemberExpr(cast<MemberExpr>(stmt));
  case Stmt::CXXMemberCallExprClass:   return visitMemberCall(cast<CXXMemberCallExpr>(stmt));
  case Stmt::ImplicitCastExprClass:    return visitImplicitCast(cast<ImplicitCastExpr>(stmt));
  case Stmt::ReturnStmtClass:          return visitReturn(cast<ReturnStmt>(stmt));
  case Stmt::IfStmtClass:              return visitIf(cast<IfStmt>(stmt));
  case Stmt::IntegerLiteralClass:      return visitLiteral(cast<IntegerLiteral>(stmt));
  case Stmt::UnaryOperatorClass:       return visitUnaryOp(cast<UnaryOperator>(stmt));
  case Stmt::ParenExprClass:           return visitParens(cast<ParenExpr>(stmt));
  case Stmt::ExprWithCleanupsClass:    return visitCleanup(cast<ExprWithCleanups>(stmt));
  case Stmt::CXXBoolLiteralExprClass:  return visitBoolLiteral(cast<CXXBoolLiteralExpr>(stmt));
  case Stmt::CStyleCastExprClass:      return visitOtherStmt(stmt); // FIXME
  case Stmt::StringLiteralClass:       return visitOtherStmt(stmt); // FIXME
  case Stmt::ConditionalOperatorClass: return visitOtherStmt(stmt); // FIXME

  case Stmt::CXXConstructExprClass: { stmt->dump(); return visitOtherStmt(stmt); }
  }

  dprintf("???visitStmt(%s)\n", stmt->getStmtClassName());
  //stmt->dump();
}

void Visitor::visitChildDecls(Stmt* stmt) {
  if (stmt == nullptr) return;
  switch(stmt->getStmtClass()) {
  case Stmt::CallExprClass:            return;
  case Stmt::CXXMemberCallExprClass:   return;
  case Stmt::CXXOperatorCallExprClass: return;

  case Stmt::CompoundStmtClass:        return;
  case Stmt::CXXThisExprClass:         return;
  case Stmt::DeclStmtClass:            for (auto decl : cast<DeclStmt>(stmt)->decls()) visit(decl); return;
  case Stmt::DeclRefExprClass:         return;
  case Stmt::BinaryOperatorClass:      return;
  case Stmt::MemberExprClass:          return;
                                       
  case Stmt::ImplicitCastExprClass:    return;
  case Stmt::ReturnStmtClass:          return;
  case Stmt::IfStmtClass:              return;
  case Stmt::IntegerLiteralClass:      return;
  case Stmt::UnaryOperatorClass:       return;
  case Stmt::ParenExprClass:           return;
  case Stmt::ExprWithCleanupsClass:    return;
  case Stmt::CXXBoolLiteralExprClass:  return;
  case Stmt::CStyleCastExprClass:      return;
  case Stmt::StringLiteralClass:       return;
  case Stmt::ConditionalOperatorClass: return;
  }

  dprintf("???visitStmtChildDecls(%s)\n", stmt->getStmtClassName());
  //stmt->dump();
}

void Visitor::visitChildStmts(Stmt* s) {
  if (s->getStmtClass() == Stmt::DeclStmtClass) {
    // child statements of a DeclStmt are nested inside the decl, don't iterate over them from here.
  }
  else {
    for (auto child : s->children()) visit(child);
  }
}

void Visitor::visitCompound(CompoundStmt* stmt)             { visitOtherStmt(stmt); }
void Visitor::visitThis(CXXThisExpr* stmt)                  { visitOtherStmt(stmt); }
void Visitor::visitDeclStmt(DeclStmt* stmt)                 { visitOtherStmt(stmt); }
void Visitor::visitDeclRef(DeclRefExpr* stmt)               { visitOtherStmt(stmt); }
void Visitor::visitBinOp(BinaryOperator* stmt)              { visitOtherStmt(stmt); }
void Visitor::visitMemberExpr(MemberExpr* stmt)             { visitOtherStmt(stmt); }

void Visitor::visitCall(CallExpr* stmt)                     { visitOtherStmt(stmt); }
void Visitor::visitMemberCall(CXXMemberCallExpr* stmt)      { visitOtherStmt(stmt); }
void Visitor::visitOpCall(CXXOperatorCallExpr* stmt)        { visitOtherStmt(stmt); }

void Visitor::visitImplicitCast(ImplicitCastExpr* stmt)     { visitOtherStmt(stmt); }
void Visitor::visitReturn(ReturnStmt* stmt)                 { visitOtherStmt(stmt); }
void Visitor::visitIf(IfStmt* stmt)                         { visitOtherStmt(stmt); }
void Visitor::visitLiteral(IntegerLiteral* stmt)            { visitOtherStmt(stmt); }
void Visitor::visitUnaryOp(UnaryOperator* stmt)             { visitOtherStmt(stmt); }
void Visitor::visitParens(ParenExpr* stmt)                  { visitOtherStmt(stmt); }
void Visitor::visitCleanup(ExprWithCleanups* stmt)          { visitOtherStmt(stmt); }
void Visitor::visitBoolLiteral(CXXBoolLiteralExpr* stmt)    { visitOtherStmt(stmt); }

void Visitor::visitOtherStmt(Stmt* stmt) {
  visitChildDecls(stmt);
  visitChildStmts(stmt);
}

//-----------------------------------------------------------------------------
