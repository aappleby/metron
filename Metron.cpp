#include "pch.h"
#include "Utils.h"

#include "MethodCollector.h"
#include "CallCollector.h"
#include "FieldCollector.h"

static llvm::cl::OptionCategory toolCategory("Metron options");

static llvm::cl::opt<string> opt_filename("o",
    llvm::cl::desc("Specify output filename"),
    llvm::cl::value_desc("filename"));

//-----------------------------------------------------------------------------

struct MetronASTConsumer : public clang::ASTConsumer {
  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Context.getTranslationUnitDecl()->dump();

    FieldCollector f(&Context);
    f.scanTranslationUnit(Context.getTranslationUnitDecl());
  }
};

//----------

struct MetronAction : public clang::ASTFrontendAction {
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::unique_ptr<clang::ASTConsumer>(new MetronASTConsumer());
  }
};

//----------

struct MetronFactory : public tooling::FrontendActionFactory {
  virtual std::unique_ptr<FrontendAction> clang::tooling::FrontendActionFactory::create(void) {
    return std::make_unique<MetronAction>();
  }
};

//-----------------------------------------------------------------------------

int main(int argc, const char** argv)
{
  tooling::CommonOptionsParser OptionsParser(argc, argv, toolCategory);
  tooling::ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
  Tool.run(new MetronFactory());
  return 0;
}

//-----------------------------------------------------------------------------