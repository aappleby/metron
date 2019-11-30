#pragma once
#include "pch.h"

//-----------------------------------------------------------------------------

class Visitor {
public:
  Visitor(ASTContext *Context) {
    context = Context;
  }

  void visitDecl(Decl* decl);
  void visitChildren(Decl* decl);

  void visitUnit(TranslationUnitDecl* unit);
  void visitRecord(RecordDecl* record);
  void visitMethod(CXXMethodDecl* method);
  void visitFunction(FunctionDecl* function);

  void dispatchStmt(Stmt* stmt);
  void visitChildren(Stmt* stmt);

  void visitBinOp(BinaryOperator* expr);
  void visitMemberExpr(MemberExpr* expr);
  void visitStmt(Stmt* stmt);

  //----------

  ASTContext* context;
};
