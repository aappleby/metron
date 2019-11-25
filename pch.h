#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#pragma warning(push)

#pragma warning(disable:4146)
#pragma warning(disable:4244)
#pragma warning(disable:4624)
#pragma warning(disable:4291)
#pragma warning(disable:4267)

#include <clang/Driver/Options.h>
#include <clang/AST/AST.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTTypeTraits.h>
#include <clang/AST/RecursiveASTVisitor.h>
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
