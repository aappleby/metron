#include "pch.h"

using namespace tooling;
using namespace llvm;
using namespace ast_matchers;
using ast_type_traits::DynTypedNode;

static cl::OptionCategory toolCategory("Metron options");
static cl::opt<string> opt_filename("o", cl::desc("Specify output filename"), cl::value_desc("filename"));

//-----------------------------------------------------------------------------

//getNameAsString()

void validateRecordField(CXXRecordDecl* record, FieldDecl* field) {
  printf("Record %s : field %s\n",
         record->getNameAsString().c_str(),
         field->getNameAsString().c_str());
}

//-----------------------------------------------------------------------------

void validateRecordMethod(CXXRecordDecl* record, CXXMethodDecl* method) {
  string name = method->getNameAsString();
  
  if (name == "tick") {
    printf("Record %s : tick method %s\n",
           record->getNameAsString().c_str(),
           method->getNameAsString().c_str());
  }
  else if (name == "tock") {
    printf("Record %s : tock method %s\n",
           record->getNameAsString().c_str(),
           method->getNameAsString().c_str());
  }
  else {
    printf("Record %s : method %s\n",
           record->getNameAsString().c_str(),
           method->getNameAsString().c_str());
  }
}

//-----------------------------------------------------------------------------

//FieldDecl
//CXXMethodDecl

void validateRecord(CXXRecordDecl* record, int depth) {


  for (auto decl : record->decls()) {
    switch (decl->getKind()) {

    case Decl::Kind::Field:
    { 
      validateRecordField(record, cast<FieldDecl>(decl));
      break;
    }

    case Decl::Kind::CXXMethod:
    {
      validateRecordMethod(record, cast<CXXMethodDecl>(decl));
      break;
    }

    default:
      break;
    }

  }
}

//-----------------------------------------------------------------------------

void validate(Decl* decl, int depth) {
  (void)depth;

  switch (decl->getKind()) {
  case Decl::Kind::CXXRecord: {
    auto d_record = cast<CXXRecordDecl>(decl);
    if (d_record->isThisDeclarationADefinition()) {
      validateRecord(d_record, depth + 1);
    }
    break;
  }
  case Decl::Kind::Namespace: {
    auto d_unit = cast<NamespaceDecl>(decl);
    for (auto d : d_unit->decls()) validate(d, depth + 1);
    break;
  }
  case Decl::Kind::TranslationUnit: {
    auto d_unit = cast<TranslationUnitDecl>(decl);
    for (auto d : d_unit->decls()) validate(d, depth + 1);
    break;
  }
  default: {
    break;
  }
  }
}

//-----------------------------------------------------------------------------

struct MyASTConsumer : public ASTConsumer {
  SourceManager* srcman = nullptr;

  void Initialize(ASTContext& ctx) override {
    srcman = &ctx.getSourceManager();
  }

  void HandleTranslationUnit(ASTContext& ctx) override {
    TranslationUnitDecl* root = ctx.getTranslationUnitDecl();
    validate(root, 0);
    return;
  }
};

//-----------------------------------------------------------------------------

struct MyFrontendAction : public ASTFrontendAction {
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &compiler, llvm::StringRef code) override {
    return std::make_unique<MyASTConsumer>();
  }
};

struct MyActionFactory : public FrontendActionFactory {

  virtual std::unique_ptr<FrontendAction> clang::tooling::FrontendActionFactory::create(void) {
    return std::make_unique<MyFrontendAction>();
  }
};

int main(int argc, const char** argv)
{
  CommonOptionsParser OptionsParser(argc, argv, toolCategory);
  ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

  ToolAction* tool = new MyActionFactory();
  Tool.run(tool);

  return 0;
}

//-----------------------------------------------------------------------------