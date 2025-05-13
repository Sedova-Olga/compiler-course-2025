#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/IR/Builders.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"

using namespace mlir;

namespace {

struct TraceLoopIterPass
    : public PassWrapper<TraceLoopIterPass, OperationPass<ModuleOp>> {
  void runOnOperation() override {
    ModuleOp module = getOperation();
    OpBuilder builder(module.getContext());

    auto traceBeginFunc = module.lookupSymbol<FuncOp>("trace_loop_iter_begin");
    auto traceEndFunc = module.lookupSymbol<FuncOp>("trace_loop_iter_end");

    builder.setInsertionPointToStart(module.getBody());
    if (!traceBeginFunc) {
      traceBeginFunc = builder.create<FuncOp>(builder.getUnknownLoc(),
                                              "trace_loop_iter_begin",
                                              builder.getFunctionType({}, {}));
      traceBeginFunc.setPrivate();
    }
    if (!traceEndFunc) {
      traceEndFunc =
          builder.create<FuncOp>(builder.getUnknownLoc(), "trace_loop_iter_end",
                                 builder.getFunctionType({}, {}));
      traceEndFunc.setPrivate();
    }

    module.walk([&](Operation *op) {
      if (auto affineFor = dyn_cast<affine::AffineForOp>(op)) {
        insertTraceCalls(affineFor, traceBeginFunc, traceEndFunc);
      }
      else if (auto scfFor = dyn_cast<scf::ForOp>(op)) {
        insertTraceCalls(scfFor, traceBeginFunc, traceEndFunc);
      }
      else if (auto scfWhile = dyn_cast<scf::WhileOp>(op)) {
        insertTraceCalls(scfWhile, traceBeginFunc, traceEndFunc);
      }
    });
  }

private:
  template <typename LoopOp>
  void insertTraceCalls(LoopOp loopOp, FuncOp traceBeginFunc,
                        FuncOp traceEndFunc) {
    OpBuilder builder(loopOp.getBody());
    builder.setInsertionPointToStart(&loopOp.getBody()->front());
    builder.create<func::CallOp>(loopOp.getLoc(), traceBeginFunc,
                                 ArrayRef<Value>{});

    builder.setInsertionPoint(loopOp.getBody()->getTerminator());
    builder.create<func::CallOp>(loopOp.getLoc(), traceEndFunc,
                                 ArrayRef<Value>{});
  }

  void insertTraceCalls(scf::WhileOp whileOp, FuncOp traceBeginFunc,
                        FuncOp traceEndFunc) {
    Block &afterBlock = whileOp.getAfter().front();
    OpBuilder builder(&afterBlock);
    builder.setInsertionPointToStart(&afterBlock);
    builder.create<func::CallOp>(whileOp.getLoc(), traceBeginFunc,
                                 ArrayRef<Value>{});
    builder.setInsertionPoint(afterBlock.getTerminator());
    builder.create<func::CallOp>(whileOp.getLoc(), traceEndFunc,
                                 ArrayRef<Value>{});
  }
};

}

namespace mlir {
void registerTraceLoopIterPass() {
  PassRegistration<TraceLoopIterPass>(
      "trace-loop-iter", "Insert calls to @trace_loop_iter_begin and "
                         "@trace_loop_iter_end on loop iterations");
}
}
