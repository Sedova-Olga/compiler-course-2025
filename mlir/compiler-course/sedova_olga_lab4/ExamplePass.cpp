#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Tools/Plugins/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace mlir;
using namespace mlir::func;

namespace {

struct ExamplePass : public PassWrapper<ExamplePass, OperationPass<ModuleOp>> {
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(ExamplePass)

  StringRef getArgument() const final { return "trace-loop-iter"; }
  StringRef getDescription() const final {
    return "Insert calls to @trace_loop_iter_begin and @trace_loop_iter_end on "
           "loop iterations";
  }

  void runOnOperation() override {
    ModuleOp module = getOperation();
    OpBuilder builder(module.getContext());

    // Попытка найти функции трассировки в модуле
    auto traceBeginFunc = module.lookupSymbol<FuncOp>("trace_loop_iter_begin");
    auto traceEndFunc = module.lookupSymbol<FuncOp>("trace_loop_iter_end");

    // Если функций нет - создаём их и добавляем в модуль
    if (!traceBeginFunc) {
      auto loc = builder.getUnknownLoc();
      auto funcType = builder.getFunctionType({}, {});
      traceBeginFunc = FuncOp::create(loc, "trace_loop_iter_begin", funcType);
      module.push_back(traceBeginFunc);
      traceBeginFunc.setPrivate();
    }

    if (!traceEndFunc) {
      auto loc = builder.getUnknownLoc();
      auto funcType = builder.getFunctionType({}, {});
      traceEndFunc = FuncOp::create(loc, "trace_loop_iter_end", funcType);
      module.push_back(traceEndFunc);
      traceEndFunc.setPrivate();
    }

    // Обход всех операций в модуле
    module.walk([&](Operation *op) {
      if (auto affineFor = dyn_cast<affine::AffineForOp>(op)) {
        insertTraceCalls(affineFor, traceBeginFunc, traceEndFunc);
      } else if (auto scfFor = dyn_cast<scf::ForOp>(op)) {
        insertTraceCalls(scfFor, traceBeginFunc, traceEndFunc);
      } else if (auto scfWhile = dyn_cast<scf::WhileOp>(op)) {
        insertTraceCalls(scfWhile, traceBeginFunc, traceEndFunc);
      }
    });
  }

private:
  template <typename LoopOp>
  void insertTraceCalls(LoopOp loopOp, FuncOp traceBeginFunc,
                        FuncOp traceEndFunc) {
    OpBuilder builder(loopOp.getContext());

    if constexpr (std::is_same_v<LoopOp, affine::AffineForOp> ||
                  std::is_same_v<LoopOp, scf::ForOp>) {
      Block *bodyBlock = loopOp.getBody();

      builder.setInsertionPointToStart(bodyBlock);
      builder.create<func::CallOp>(loopOp.getLoc(), traceBeginFunc,
                                   ArrayRef<Value>{});

      builder.setInsertionPoint(bodyBlock->getTerminator());
      builder.create<func::CallOp>(loopOp.getLoc(), traceEndFunc,
                                   ArrayRef<Value>{});

    } else if constexpr (std::is_same_v<LoopOp, scf::WhileOp>) {
      // Получаем регион тела цикла (after-region)
      Region &afterRegion = loopOp.getAfter();

      // Проверяем, что регион не пустой
      assert(!afterRegion.empty() && "After region should not be empty");

      // Получаем первый блок региона
      Block &bodyBlockRef = afterRegion.front();
      Block *bodyBlock = &bodyBlockRef;

      OpBuilder builder(loopOp.getContext());

      builder.setInsertionPointToStart(bodyBlock);
      builder.create<func::CallOp>(loopOp.getLoc(), traceBeginFunc,
                                   ArrayRef<Value>{});

      builder.setInsertionPoint(bodyBlock->getTerminator());
      builder.create<func::CallOp>(loopOp.getLoc(), traceEndFunc,
                                   ArrayRef<Value>{});
    }

  }
};


} // namespace

// Объявление и определение типа для пасса
MLIR_DECLARE_EXPLICIT_TYPE_ID(ExamplePass)
MLIR_DEFINE_EXPLICIT_TYPE_ID(ExamplePass)

// Регистрация пасса в pipeline
static mlir::PassPipelineRegistration<>
    pipeline("ExamplePass_Sedova_Olga_FIIT1_MLIR",
             "Pipeline that runs ExamplePass", [](mlir::OpPassManager &pm) {
               pm.addPass(std::make_unique<ExamplePass>());
             });

// Функции для регистрации плагина
mlir::PassPluginLibraryInfo getTraceLoopIterPassPluginInfo() {
  return {MLIR_PLUGIN_API_VERSION, "ExamplePass_Sedova_Olga_FIIT1_MLIR", "1.0",
          []() { mlir::PassRegistration<ExamplePass>(); }};
}

extern "C" LLVM_ATTRIBUTE_WEAK mlir::PassPluginLibraryInfo
mlirGetPassPluginInfo() {
  return getTraceLoopIterPassPluginInfo();
}
