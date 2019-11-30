#include "pch.h"
#include "Visitor.h"
#include "Utils.h"

//-----------------------------------------------------------------------------

void Visitor::visitDecl(Decl* decl) {
  if (decl == nullptr) return;
  switch(decl->getKind()) {
  case Decl::Kind::TranslationUnit: return visitUnit(cast<TranslationUnitDecl>(decl));
  case Decl::Kind::CXXRecord:       return visitRecord(cast<RecordDecl>(decl));
  case Decl::Kind::Record:          return visitRecord(cast<RecordDecl>(decl));
  case Decl::Kind::CXXMethod:       return visitMethod(cast<CXXMethodDecl>(decl));
  case Decl::Kind::Function:        return visitFunction(cast<FunctionDecl>(decl));
  }

  dprintf("decl %s\n", decl->getDeclKindName());
  visitChildren(decl);
}

//----------------------------------------

void Visitor::visitChildren(Decl* decl) {
  Indenter indent;
  if (auto context = dyn_cast<DeclContext>(decl)) {
    for (auto child : context->decls()) visitDecl(child);
  }
}

//----------------------------------------

void Visitor::visitUnit(TranslationUnitDecl* unit) {
  dprintf("visitUnit\n");
  visitChildren(unit);
}

//----------

void Visitor::visitRecord(RecordDecl* record) {
  dprintf("visitRecord %s\n", record->getNameAsString().c_str());
  visitChildren(record);
}

//----------

void Visitor::visitMethod(CXXMethodDecl* method) {
  dprintf("visitMethod %s\n", method->getNameAsString().c_str());
  visitChildren(method);
}

//----------

void Visitor::visitFunction(FunctionDecl* function) {
  dprintf("visitFunction %s\n", function->getNameAsString().c_str());
  visitChildren(function);
}

//-----------------------------------------------------------------------------

void Visitor::visitStmt(Stmt* stmt) {
  if (stmt == nullptr) return;
  switch(stmt->getStmtClass()) {
  case Stmt::BinaryOperatorClass: return visitBinOp(cast<BinaryOperator>(stmt));
  case Stmt::MemberExprClass:     return visitMemberExpr(cast<MemberExpr>(stmt));
  }

  dprintf("visitStmt %s\n", stmt->getStmtClassName());
  visitChildren(stmt);
}

//----------------------------------------

void Visitor::visitChildren(Stmt* s) {
  Indenter indent;
  for (auto s2 : s->children()) visitStmt(s2);
}

//----------------------------------------

void Visitor::visitBinOp(BinaryOperator* expr) {
  dprintf("visitBinOp %s\n", expr->getStmtClassName());
  visitChildren(expr);
}

//----------

void Visitor::visitMemberExpr(MemberExpr* expr) {
  dprintf("visitMemberExpr %s\n", expr->getStmtClassName());
  visitChildren(expr);
}

//-----------------------------------------------------------------------------
