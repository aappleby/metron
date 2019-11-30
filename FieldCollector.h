#pragma once
#include "pch.h"
#include "Visitor.h"

typedef vector<FieldDecl*> FieldPath;
typedef vector<FieldPath>  FieldPaths;
typedef set<FieldDecl*>    FieldSet;

typedef map<FieldDecl*, FieldSet>    FieldMap;
typedef map<RecordDecl*, FieldPaths> RecordToFieldPaths;

//-----------------------------------------------------------------------------

class FieldCollector : public Visitor {
public:
  typedef Visitor base;

  FieldCollector(ASTContext* context_);

  //----------

  void scanTranslationUnit(TranslationUnitDecl* unit);

  //----------

  void visitRecord(RecordDecl* record);
  void visitMethod(CXXMethodDecl* method);

  void visitBinOp(BinaryOperator* expr);
  void visitMemberExpr(MemberExpr* expr);

  //----------

  set<RecordDecl*>       rootRecords;
  vector<RecordDecl*>    recordStack;
  vector<CXXMethodDecl*> methodStack;
  vector<FunctionDecl*>  callStack;
  vector<MemberExpr*>    memberStack;

  FieldPath currentPath;

};

//-----------------------------------------------------------------------------
