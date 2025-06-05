#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "fma-decompose"

namespace {
class FMADecomposePass : public MachineFunctionPass {
public:
  static char ID;
  FMADecomposePass() : MachineFunctionPass(ID) {
    initializeFMADecomposePassPass(*PassRegistry::getPassRegistry());
  }

  bool runOnMachineFunction(MachineFunction &MF) override {
    bool Changed = false;
    MachineRegisterInfo &MRI = MF.getRegInfo();

    for (auto &MBB : MF) {
      for (auto MII = MBB.begin(), E = MBB.end(); MII != E;) {
        MachineInstr &MI = *MII++;
        // Проверяем, что инструкция — FMA
        if (MI.getOpcode() == TargetOpcode::FMA) {
          LLVM_DEBUG(dbgs() << "Decomposing FMA: " << MI);

          // Предполагаем, что операнды: dst, src1, src2, src3
          // FMA dst, src1, src2, src3  => dst = src3 + (src1 * src2)
          Register Dst = MI.getOperand(0).getReg();
          Register Src1 = MI.getOperand(1).getReg();
          Register Src2 = MI.getOperand(2).getReg();
          Register Src3 = MI.getOperand(3).getReg();

          // Создаём временный регистр для MUL
          Register MulReg = MRI.createVirtualRegister(MRI.getRegClass(Dst));

          // Вставляем MUL перед FMA
          BuildMI(MBB, MI, MI.getDebugLoc(),
                  MF.getSubtarget().getInstrInfo()->get(TargetOpcode::MUL),
                  MulReg)
              .addReg(Src1)
              .addReg(Src2);

          // Вставляем ADD перед FMA
          BuildMI(MBB, MI, MI.getDebugLoc(),
                  MF.getSubtarget().getInstrInfo()->get(TargetOpcode::ADD), Dst)
              .addReg(Src3)
              .addReg(MulReg);

          // Удаляем исходную FMA
          MI.eraseFromParent();

          Changed = true;
        }
      }
    }

    return Changed;
  }

  StringRef getPassName() const override { return "FMA Decompose Pass"; }
};

char FMADecomposePass::ID = 0;

} // end anonymous namespace

static RegisterPass<FMADecomposePass> X("fma-decompose", "Decompose FMA into MUL + ADD",
                                        false,
                                   false);
