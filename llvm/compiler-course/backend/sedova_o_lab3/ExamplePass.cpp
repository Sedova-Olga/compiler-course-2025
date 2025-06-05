#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "fma-decompose"

namespace {
class FMADecomposePass : public MachineFunctionPass {
public:
  static char ID;
  FMADecomposePass() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override {
    bool Changed = false;
    MachineRegisterInfo &MRI = MF.getRegInfo();

    const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();

    for (auto &MBB : MF) {
      for (auto MII = MBB.begin(), E = MBB.end(); MII != E;) {
        MachineInstr &MI = *MII++;
        if (MI.getOpcode() == TargetOpcode::G_FMA) {
          LLVM_DEBUG(dbgs() << "Decomposing FMA: " << MI);

          Register Dst = MI.getOperand(0).getReg();
          Register Src1 = MI.getOperand(1).getReg();
          Register Src2 = MI.getOperand(2).getReg();
          Register Src3 = MI.getOperand(3).getReg();

          Register MulReg = MRI.createVirtualRegister(MRI.getRegClass(Dst));

          BuildMI(MBB, MI, MI.getDebugLoc(), TII->get(TargetOpcode::G_MUL),
                  MulReg)
              .addReg(Src1)
              .addReg(Src2);

          BuildMI(MBB, MI, MI.getDebugLoc(), TII->get(TargetOpcode::G_ADD), Dst)
              .addReg(Src3)
              .addReg(MulReg);

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

namespace llvm {
INITIALIZE_PASS(FMADecomposePass, "fma-decompose",
                "Decompose FMA into MUL + ADD", false, false)
} // namespace llvm

static RegisterPass<FMADecomposePass>
    X("fma-decompose", "Decompose FMA into MUL + ADD", false, false);
