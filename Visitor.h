#pragma once
#include "pch.h"

//-----------------------------------------------------------------------------

class Visitor {
public:

  typedef function<void(Decl*)> DeclCallback;
  typedef function<void(Stmt*)> StmtCallback;

  static void visit(Decl* root, Stmt::StmtClass filter, StmtCallback callback) {
    Visitor v(filter, callback);
    v.visit(root);
  }

  static void visit(Decl* root, Decl::Kind filter, DeclCallback callback) {
    Visitor v(filter, callback);
    v.visit(root);
  }

  static void visit(Stmt* root, Stmt::StmtClass filter, StmtCallback callback) {
    Visitor v(filter, callback);
    v.visit(root);
  }

  static void visit(Stmt* root, Decl::Kind filter, DeclCallback callback) {
    Visitor v(filter, callback);
    v.visit(root);
  }

  Visitor() {}

  Visitor(Decl::Kind filter, DeclCallback callback) {
    declCallbackMap[filter] = callback;
  }

  Visitor(Stmt::StmtClass filter, StmtCallback callback) {
    stmtCallbackMap[filter] = callback;
  }

  virtual void visit(Decl* decl) final;
  virtual void visitChildDecls(Decl* decl) final;
  virtual void visitChildStmts(Decl* decl) final;

  virtual void visitChildren(Decl* decl) final {
    visitChildDecls(decl);
    visitChildStmts(decl);
  }

  virtual void visit(Stmt* stmt) final;
  virtual void visitChildDecls(Stmt* stmt) final;
  virtual void visitChildStmts(Stmt* stmt) final;
  virtual void visitChildren(Stmt* stmt) final {
    visitChildDecls(stmt);
    visitChildStmts(stmt);
  }

  virtual void visitUnit        (TranslationUnitDecl* unit);
  virtual void visitCXXRecord   (CXXRecordDecl* record);
  virtual void visitRecord      (RecordDecl* record);
  virtual void visitMethod      (CXXMethodDecl* method);
  virtual void visitFunction    (FunctionDecl* function);
  virtual void visitVar         (VarDecl* decl);
  virtual void visitParmVar     (ParmVarDecl* decl);
  virtual void visitField       (FieldDecl* decl);
  virtual void visitTypedef     (TypedefDecl* decl);
  virtual void visitConversion  (CXXConversionDecl* decl);
  virtual void visitOtherDecl   (Decl* decl);

  virtual void visitThis        (CXXThisExpr* expr);
  virtual void visitCompound    (CompoundStmt* stmt);
  virtual void visitReturn      (ReturnStmt* stmt);
  virtual void visitIf          (IfStmt* stmt);
  virtual void visitParens      (ParenExpr* stmt);

  virtual void visitDeclStmt    (DeclStmt* decl);
  virtual void visitDeclRef     (DeclRefExpr* expr);
  
  virtual void visitMemberExpr  (MemberExpr* expr);

  virtual void visitCall        (CallExpr* expr);
  virtual void visitMemberCall  (CXXMemberCallExpr* expr);
  virtual void visitOpCall      (CXXOperatorCallExpr* expr);

  virtual void visitImplicitCast(ImplicitCastExpr* expr);

  virtual void visitLiteral     (IntegerLiteral* stmt);
  virtual void visitBinOp       (BinaryOperator* expr);
  virtual void visitUnaryOp     (UnaryOperator* stmt);

  virtual void visitExprWithCleanups(ExprWithCleanups* stmt);
  virtual void visitOtherStmt   (Stmt* stmt);

  //----------

  ASTContext* context;
  map<Decl::Kind, DeclCallback> declCallbackMap;
  map<Stmt::StmtClass, StmtCallback> stmtCallbackMap;
};
