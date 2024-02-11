//===-- LC3FrameLowering.cpp - LC3 Frame Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the LC3 implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "LC3FrameLowering.h"
#include "LC3InstrInfo.h"
#include "LC3MachineFunctionInfo.h"
#include "LC3Subtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

#define DEBUG_TYPE "framelowering"

// hasFP - Return true if the specified function should have a dedicated frame
// pointer register.  This is true if the function has variable sized allocas,
// if it needs dynamic stack realignment, if frame pointer elimination is
// disabled, or if the frame address is taken.
bool LC3FrameLowering::hasFP(const MachineFunction &MF) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  const TargetRegisterInfo *TRI = MF.getSubtarget().getRegisterInfo();

  return MF.getTarget().Options.DisableFramePointerElim(MF) ||
      MFI.hasVarSizedObjects() || MFI.isFrameAddressTaken() ||
      TRI->hasStackRealignment(MF);
}

// Adjsut StackPointer by Amount bytes.
void LC3FrameLowering::adjustStackPtr(MachineFunction &MF,
                                        MachineBasicBlock &MBB,
                                        MachineBasicBlock::iterator MBBI,
                                        int16_t Amount
                                        ) const {
  // BuildMI(MachineBasicBlock, MachineInstr, DebugLoc/PCSection, MCInstDesc, DstReg).addReg().addReg/Imm()
  
  DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  const LC3Subtarget &Subtarget = MF.getSubtarget<LC3Subtarget>();
  const LC3InstrInfo &TII = *static_cast<const LC3InstrInfo *>(Subtarget.getInstrInfo());

  if (isInt<16>(Amount)) // ADD SP, SP, Imm5
  {
    BuildMI(MBB, MBBI, DL, TII.get(LC3::ADDri), LC3::SP).addReg(LC3::SP).addImm(Amount);
  }
  else { // ADD SP, SP, Reg
    Register Reg = STI.getInstrInfo()->loadImmediate(MBB, MBBI, DL, Amount);
    BuildMI(MBB, MBBI, DL, TII.get(LC3::ADDrr), LC3::SP).addReg(LC3::SP).addReg(Reg, RegState::Kill);
  }

} // adjustStackPtr()

// Eliminate ADJCALLSTACKDOWN, ADJCALLSTACKUP pseudo instructions
MachineBasicBlock::iterator LC3FrameLowering::
eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator I) const {
  // dynamic alloc
  if (!hasReservedCallFrame(MF)) {
  MachineInstr &MI = *I;
    int Size = MI.getOperand(0).getImm();
    if (MI.getOpcode() == LC3::ADJCALLSTACKDOWN)
      Size = -Size;

    if (Size)
      adjustStackPtr(MF, MBB, I, Size);
  }
  return MBB.erase(I);
} // eliminateCallFramePseudoInstr()