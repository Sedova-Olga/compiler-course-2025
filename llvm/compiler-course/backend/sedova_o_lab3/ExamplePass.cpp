#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
struct DecomposeFMAPass : public MachineFunctionPass {
  static char ID;
  DecomposeFMAPass() : MachineFunctionPass(ID) {}
  DecomposeFMAPass() : MachineFunctionPass(ID) {
    initializeDecomposeFMAPassPass(*PassRegistry::getPassRegistry());
  }

  StringRef getPassName() const override {
    return "Decompose FMA instructions";
  }

  bool runOnMachineFunction(MachineFunction &MF) override {
    const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
    MachineRegisterInfo &MRI = MF.getRegInfo();
    bool Changed = false;

    for (auto &MBB : MF) {
      for (auto MI = MBB.begin(), ME = MBB.end(); MI != ME;) {
        MachineInstr &Instr = *MI++;
        if (Instr.getOpcode() == TargetOpcode::G_FMA) {
          Register Dest = Instr.getOperand(0).getReg();
          Register Src0 = Instr.getOperand(1).getReg();
          Register Src1 = Instr.getOperand(2).getReg();
          Register Src2 = Instr.getOperand(3).getReg();

          Register MulRes = MRI.createVirtualRegister(MRI.getRegClass(Dest));
          Register AddRes = MRI.createVirtualRegister(MRI.getRegClass(Dest));

          // %mul = MUL %src0, %src1
          BuildMI(MBB, Instr, Instr.getDebugLoc(),
                  TII->get(TargetOpcode::G_MUL), MulRes)
              .addReg(Src0)
              .addReg(Src1);

          // %add = ADD %src2, %mul
          BuildMI(MBB, Instr, Instr.getDebugLoc(),
                  TII->get(TargetOpcode::G_ADD), AddRes)
              .addReg(Src2)
              .addReg(MulRes);

          MRI.replaceRegWith(Dest, AddRes);
          Instr.eraseFromParent();
          Changed = true;
        }
      }
    }
    return Changed;
  }
};
char DecomposeFMAPass::ID = 0;
} // namespace

INITIALIZE_PASS(DecomposeFMAPass, "decompose-fma", "Decompose FMA instructions",
                false, false)

extern "C" LLVM_EXTERNAL_VISIBILITY void
LLVMInitializeDecomposeFMAPassPass(PassRegistry &Registry) {
  initializeDecomposeFMAPassPass(Registry);
}

extern "C" LLVM_EXTERNAL_VISIBILITY Pass *createDecomposeFMAPass() {
  return new DecomposeFMAPass();
} 
