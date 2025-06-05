#include "X86.h"
#include "X86InstrInfo.h"
#include "X86Subtarget.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/ADT/Optional.h"

#define DEBUG_TYPE "fma-decompose-x86"

using namespace llvm;

namespace {

struct FMAGroup {
  ArrayRef<unsigned> Opcodes;
  unsigned MulOpcode;
  unsigned AddOpcode;
};

static const FMAGroup FMA_Groups[] = {
    {{X86::VFMADD132PSr, X86::VFMADD213PSr, X86::VFMADD231PSr},
     X86::MULPSrr,
     X86::ADDPSrr},
    {{X86::VFMADD132PDr, X86::VFMADD213PDr, X86::VFMADD231PDr},
     X86::MULPDrr,
     X86::ADDPDrr},
    {{X86::VFMADD132SSr, X86::VFMADD213SSr, X86::VFMADD231SSr},
     X86::MULSSrr,
     X86::ADDSSrr},
    {{X86::VFMADD132SDr, X86::VFMADD213SDr, X86::VFMADD231SDr},
     X86::MULSDrr,
     X86::ADDSDrr},
};

class FMADecomposePass : public MachineFunctionPass {
public:
  static char ID;
  FMADecomposePass() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override {
    const X86Subtarget &ST = MF.getSubtarget<X86Subtarget>();
    if (!ST.hasFMA()) {
      LLVM_DEBUG(dbgs() << "FMA not supported on this subtarget\n");
      return false;
    }

    const X86InstrInfo *TII = ST.getInstrInfo();
    MachineRegisterInfo &MRI = MF.getRegInfo();
    bool Changed = false;

    for (MachineBasicBlock &MBB : MF) {
      SmallVector<MachineInstr *, 8> WorkList;

      for (MachineInstr &MI : make_early_inc_range(MBB)) {
        if (auto FMAData = getFMAInfo(MI.getOpcode())) {
          WorkList.push_back(&MI);
        }
      }

      for (MachineInstr *MI : WorkList) {
        Changed |= decomposeFMA(*MI, MBB, TII, MRI);
      }
    }

    return Changed;
  }

private:
  struct FMAData {
    unsigned MulOp;
    unsigned AddOp;
    unsigned FMAIndex;
    const FMAGroup *Group;
  };

  Optional<FMAData> getFMAInfo(unsigned Opcode) const {
    for (const auto &Group : FMA_Groups) {
      for (unsigned i = 0; i < Group.Opcodes.size(); ++i) {
        if (Group.Opcodes[i] == Opcode) {
          return FMAData{Group.MulOpcode, Group.AddOpcode, i, &Group};
        }
      }
    }
    return None;
  }

  bool decomposeFMA(MachineInstr &MI, MachineBasicBlock &MBB,
                    const X86InstrInfo *TII, MachineRegisterInfo &MRI) {
    auto FMAInfoOpt = getFMAInfo(MI.getOpcode());
    if (!FMAInfoOpt)
      return false;

    const FMAData &Info = *FMAInfoOpt;
    DebugLoc DL = MI.getDebugLoc();

    Register Dst = MI.getOperand(0).getReg();
    Register Op1 = MI.getOperand(1).getReg();
    Register Op2 = MI.getOperand(2).getReg();
    Register Op3 = MI.getOperand(3).getReg();

    Register MulLHS, MulRHS, AddSrc;
    switch (Info.FMAIndex) {
    case 0:
      MulLHS = Op1;
      MulRHS = Op3;
      AddSrc = Op2;
      break;
    case 1:
      MulLHS = Op1;
      MulRHS = Op2;
      AddSrc = Op3;
      break;
    case 2:
      MulLHS = Op2;
      MulRHS = Op3;
      AddSrc = Op1;
      break;
    default:
      llvm_unreachable("Invalid FMA index");
    }

    const TargetRegisterClass *RC = MRI.getRegClass(MulLHS);
    Register TmpReg = MRI.createVirtualRegister(RC);

    BuildMI(MBB, MI, DL, TII->get(Info.MulOp), TmpReg)
        .addReg(MulLHS)
        .addReg(MulRHS)
        .setMIFlag(MachineInstr::MIFlag::NoFPExcept);

    BuildMI(MBB, MI, DL, TII->get(Info.AddOp), Dst)
        .addReg(AddSrc)
        .addReg(TmpReg)
        .setMIFlag(MachineInstr::MIFlag::NoFPExcept);

    MI.eraseFromParent();
    return true;
  }
};

char FMADecomposePass::ID = 0;

} // namespace

static RegisterPass<FMADecomposePass>
    X("fma-decompose-x86", "Decompose x86 FMA instructions into MUL + ADD",
      false, false);
