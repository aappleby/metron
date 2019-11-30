#include "pch.h"
#include "FieldCollector.h"
#include "Utils.h"

//-----------------------------------------------------------------------------

FieldCollector::FieldCollector(ASTContext* context_) : base(context_) {
  recordStack.push_back(nullptr);
}

//-----------------------------------------------------------------------------

void FieldCollector::scanTranslationUnit(TranslationUnitDecl* unit) {
  dprintf("scanTranslationUnit\n");
  Indenter indent;
  visitUnit(unit);
}

//-----------------------------------------------------------------------------

void FieldCollector::visitRecord(RecordDecl* record) {
  dprintf("record\n");
  dprintf(record->isThisDeclarationADefinition() ? "def\n" : "no def\n");
  recordStack.push_back(record);
  base::visitChildren(record);
  recordStack.pop_back();
}

//----------

void FieldCollector::visitMethod(CXXMethodDecl* method) {
  if (recordStack.back()) {
    dprintf("method\n");
  } else {
    dprintf("method outside of record\n");
  }
  dprintf(method->isThisDeclarationADefinition() ? "def\n" : "no def\n");
  dprintf(method->hasBody() ? "body\n" : "no body\n");

  methodStack.push_back(method);
  base::visitChildren(method);
  methodStack.pop_back();
}

//-----------------------------------------------------------------------------

void FieldCollector::visitBinOp(BinaryOperator* op) {
  if (op->isAssignmentOp() || op->isCompoundAssignmentOp()) {
    dprintf("assignment op\n");
    dprintf("RHS\n");
    base::visitStmt(op->getRHS());
    dprintf("LHS\n");
    base::visitStmt(op->getLHS());
  }
  else {
    base::visitChildren(op);
  }
}

//----------

void FieldCollector::visitMemberExpr(MemberExpr* expr) {
  base::visitMemberExpr(expr);
  /*
  auto member = expr->getMemberDecl();
  dprintf("**** %s %s\n", expr->getStmtClassName(), member->getNameAsString().c_str());

  std::vector<FieldDecl*> fields = memberExprToPath(expr);

  for (auto f : fields) {
    auto ft = f->getType();
    if (ft->isRecordType()) {
      RecordDecl* record = ft->getAsCXXRecordDecl();
      dprintf("%s %s %s\n", ft->getTypeClassName(), record->getNameAsString().c_str(), f->getNameAsString().c_str());
    }
    else {
      dprintf("%s %s\n", ft->getTypeClassName(), f->getNameAsString().c_str());
    }
  }

  auto range = pathToRange(context, fields);
  dprintf("offset %d width %d\n", range.first, range.second);
  */
}

//-----------------------------------------------------------------------------

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
#if 0
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
#endif
  return result;
}

//-----------------------------------------------------------------------------
