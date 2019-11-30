#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#pragma warning(disable:4100)
#pragma warning(disable:4146)
#pragma warning(disable:4244)
#pragma warning(disable:4245)
#pragma warning(disable:4267)
#pragma warning(disable:4291)
#pragma warning(disable:4324)
#pragma warning(disable:4389)
#pragma warning(disable:4456)
#pragma warning(disable:4458)
#pragma warning(disable:4624)
#pragma warning(disable:4702)
#pragma warning(disable:4996)
#pragma warning(disable:5054)

#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/Expr.h>
#include <clang/AST/ExprCXX.h>
#include <clang/AST/LexicallyOrderedRecursiveASTVisitor.h>
#include <clang/AST/RecordLayout.h>
#include <clang/AST/RecursiveASTVisitor.h>

#include <clang/Frontend/CompilerInstance.h>

#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

using namespace std;
using namespace clang;

typedef set<FunctionDecl*> FunctionSet;

/*

#pragma warning(disable:4100)
#pragma warning(disable:4457)
#pragma warning(disable:4459)


#pragma warning(push)

#pragma warning(disable:4244)
#pragma warning(disable:4624)
#pragma warning(disable:4291)
#pragma warning(disable:4458)
#pragma warning(disable:4100)
#pragma warning(disable:4245)
#pragma warning(disable:5054)
#pragma warning(disable:4324)
#pragma warning(disable:4389)
#pragma warning(disable:4996)
#pragma warning(disable:4702)

#include <clang/Driver/Options.h>
#include <clang/AST/AST.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTTypeTraits.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/LexicallyOrderedRecursiveASTVisitor.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Frontend/ASTConsumers.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Rewrite/Core/Rewriter.h>

#pragma warning(pop)

using namespace std;
using namespace clang;
*/
