#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

#include <map>
#include <string>

using namespace clang;
using namespace clang::tooling;

namespace {

class ImplicitCastVisitor : public RecursiveASTVisitor<ImplicitCastVisitor> {
public:
  explicit ImplicitCastVisitor(ASTContext *Context)
      : Context(Context), CurrentFunction(nullptr) {}

  bool VisitFunctionDecl(FunctionDecl *FD) {
    if (FD->hasBody()) {
      CurrentFunction = FD;
      CastCounts.clear();
      TraverseStmt(FD->getBody());

      llvm::outs() << "Function `" << FD->getNameAsString() << "`\n";
      for (const auto &Pair : CastCounts) {
        llvm::outs() << Pair.first << ": " << Pair.second << "\n";
      }
      llvm::outs() << "\n";

      CurrentFunction = nullptr;
      CastCounts.clear();

      // Ќе нужно дальше обходить тело, т.к. уже сделали TraverseStmt
      return false;
    }
    return true;
  }

  bool VisitImplicitCastExpr(ImplicitCastExpr *ICE) {
    if (!CurrentFunction)
      return true;

    auto castKind = ICE->getCastKind();
    std::string castStr = castKindToString(castKind);
    if (castStr.empty())
      return true;

    QualType srcType = ICE->getSubExpr()->getType();
    QualType dstType = ICE->getType();

    // »гнорируем касты без изменени€ типа
    if (Context->hasSameType(srcType, dstType))
      return true;

    std::string srcTypeStr = srcType.getAsString();
    std::string dstTypeStr = dstType.getAsString();

    std::string key = srcTypeStr + " -> " + dstTypeStr;
    CastCounts[key]++;

    return true;
  }

private:
  ASTContext *Context;
  FunctionDecl *CurrentFunction;
  std::map<std::string, unsigned> CastCounts;

  // ѕреобразуем CastKind в строку, только дл€ интересующих кастов
  std::string castKindToString(CastKind kind) {
    switch (kind) {
    case CK_IntegralToFloating:
      return "IntegralToFloating";
    case CK_FloatingToIntegral:
      return "FloatingToIntegral";
    case CK_FloatingCast:
      return "FloatingCast";
    case CK_IntegralCast:
      return "IntegralCast";
    case CK_NoOp:
      return ""; // не считаем
    default:
      return "";
    }
  }
};

class ImplicitCastConsumer : public ASTConsumer {
public:
  explicit ImplicitCastConsumer(ASTContext *Context) : Visitor(Context) {}

  virtual void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  ImplicitCastVisitor Visitor;
};

class ImplicitCastAction : public ASTFrontendAction {
public:
  virtual std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &Compiler,
                    llvm::StringRef InFile) override {
    return std::make_unique<ImplicitCastConsumer>(&Compiler.getASTContext());
  }
};

} // namespace

int main(int argc, const char **argv) {
  llvm::cl::OptionCategory MyToolCategory("implicit-cast-counter options");
  auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
  if (!ExpectedParser) {
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser &OptionsParser = ExpectedParser.get();

  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  return Tool.run(newFrontendActionFactory<ImplicitCastAction>().get());
}


static clang::FrontendPluginRegistry::Add<ExampleAction>
    X("Lab1", "Description plugin");
