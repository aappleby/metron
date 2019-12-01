#include "pch.h"
#include "Utils.h"

//-----------------------------------------------------------------------------

int depth = 0;

void indent() {
  depth += 2;
}

void dedent() {
  depth -= 2;
}

void dprintf(const char* format, ...) {
  for(int i = 0; i < depth; i++) putchar(' ');
  va_list args;
  va_start (args, format);
  vprintf (format, args);
  va_end (args);
}

void sprintf(std::string& out, const char* format, ...)
{
  char source_buf[1024];

  va_list args;
  va_start (args, format);
  vsnprintf(source_buf, 1024, format, args);
  va_end(args);

  out.append(source_buf);
}

//-----------------------------------------------------------------------------

std::string toString(Decl* decl) {
  if (decl == nullptr) return "<decl null>";

  switch(decl->getKind()) {
   
  case Decl::Kind::Field: {
    FieldDecl* f = cast<FieldDecl>(decl);
    return f->getParent()->getNameAsString() + "::" + f->getNameAsString();
  }

  case Decl::Kind::CXXConversion:
  case Decl::Kind::CXXMethod: {
    CXXMethodDecl* m = cast<CXXMethodDecl>(decl);
    return m->getParent()->getNameAsString() + "::" + m->getNameAsString();
  }

  case Decl::Kind::CXXRecord: {
    return cast<CXXRecordDecl>(decl)->getNameAsString();
  }

  case Decl::Kind::Function: {
    return cast<FunctionDecl>(decl)->getNameAsString();
  }

  default: {
    dprintf("xxxxx\n");
    decl->dump();
    return "<derp>";
  }

  }
}

//-----------------------------------------------------------------------------

std::string toString(Stmt* stmt) {
  if (stmt == nullptr) return "<stmt null>";

  switch(stmt->getStmtClass()) {

  case Stmt::MemberExprClass: {
    MemberExpr* s_member = cast<MemberExpr>(stmt);
    return toString(s_member->getMemberDecl());
  }

  case Stmt::CXXMemberCallExprClass: {
    CXXMemberCallExpr* s_call = cast<CXXMemberCallExpr>(stmt);
    return toString(s_call->getImplicitObjectArgument()) + "." +  toString(s_call->getDirectCallee()) + "()";
  }

  case Stmt::CallExprClass:
  {
    CallExpr* s_call = cast<CallExpr>(stmt);
    return toString(s_call->getDirectCallee()) + "()";
  }

  default:
    dprintf("xxxxx\n");
    stmt->dump();
    return stmt->getStmtClassName();
  }
}

//-----------------------------------------------------------------------------
