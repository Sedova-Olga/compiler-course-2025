#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/raw_ostream.h"
#include "X86.h"
#include "X86InstrInfo.h"
#include "X86Subtarget.h"

using namespace llvm;

namespace {

class DecomposeFMAPass : public llvm::MachineFunctionPass {
public:
  static char ID;
  DecomposeFMAPass() : llvm::MachineFunctionPass(ID) {}

  StringRef getPassName() const override {
    return "Decompose generic FMA instructions into MUL + ADD";
  }

  bool runOnMachineFunction(MachineFunction &MF) override {
    const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
    MachineRegisterInfo &MRI = MF.getRegInfo();
    bool Changed = false;

    for (auto &MBB : MF) {
      for (auto MI = MBB.begin(), ME = MBB.end(); MI != ME;) {
        MachineInstr &Instr = *MI++;
        // Ищем generic FMA (G_FMA)
        if (Instr.getOpcode() == TargetOpcode::G_FMA) {
          // Формат: dst = G_FMA src0, src1, src2  (dst = src2 + src0 * src1)
          Register Dest = Instr.getOperand(0).getReg();
          Register Src0 = Instr.getOperand(1).getReg();
          Register Src1 = Instr.getOperand(2).getReg();
          Register Src2 = Instr.getOperand(3).getReg();

          // Создаем временный виртуальный регистр для результата MUL
          Register MulRes = MRI.createVirtualRegister(MRI.getRegClass(Dest));

          // Вставляем MUL: MulRes = Src0 * Src1
          BuildMI(MBB, Instr, Instr.getDebugLoc(),
                  TII->get(TargetOpcode::G_MUL), MulRes)
              .addReg(Src0)
              .addReg(Src1);

          // Вставляем ADD: Dest = Src2 + MulRes
          BuildMI(MBB, Instr, Instr.getDebugLoc(),
                  TII->get(TargetOpcode::G_ADD), Dest)
              .addReg(Src2)
              .addReg(MulRes);

          // Заменяем все использования Dest на новый результат ADD
          MRI.replaceRegWith(Dest, Dest);

          // Удаляем исходную FMA-инструкцию
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

static llvm::RegisterPass<DecomposeFMAPass>
    X("decompose-fma", "Decompose FMA instructions", false, false);