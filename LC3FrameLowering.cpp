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
#include "llvm/IR/PrintPasses.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetOptions.h"
#include <cstdint>

using namespace llvm;

#define DEBUG_TYPE "framelowering"

// Note: TargetFrameLowering() LAO := LocalAreaOffset
LC3FrameLowering::LC3FrameLowering(const LC3Subtarget &STI)
    : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 
                          Align(16), 0, Align(16)) {}

/* 
  emitPrologue()

  callee:
    push FP  (assumed it is done...<---Question)
    FP <- SP
    SP <- SP - Local_Alloca_(StackSize)
    <callee job>
*/
void LC3FrameLowering::emitPrologue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {
  
  const LC3Subtarget &STI = MF.getSubtarget<LC3Subtarget>();
  const LC3InstrInfo &TII = *STI.getInstrInfo();
  const LC3RegisterInfo &RegInfo = *STI.getRegisterInfo();
  
  LC3MachineFunctionInfo *FuncInfo = MF.getInfo<LC3MachineFunctionInfo>();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc DL = (MBBI != MBB.end()) ? MBBI->getDebugLoc() : DebugLoc();

  uint16_t StackSize = MFI.getStackSize();

  if (StackSize == 0 && !MFI.adjustsStack()) return;

  // Question: did we 'push FP' on the Stack?
  
  // if hasFP, then set to FP := SP
  if (hasFP(MF))
    BuildMI(MBB, MBBI, DL, TII.get(LC3::ADDrr), LC3::FP)
      .addReg(LC3::SP).setMIFlag(MachineInstr::FrameSetup);

  // .cfi_def_cfa defines a rule for computing CFA as: take address from Register and add Offset to it.
  // CFI is a mechanism for checking an invalid/odd control-flow behaviour.
  // Emit ".cfi_def_cfa_offset StackSize"
  BuildCFI(MBB, MBBI, DL, TII, MCCFIInstruction::cfiDefCfaOffset(nullptr, StackSize));

  // Callee Saved
  const std::vector<CalleeSavedInfo> &CSI = MFI.getCalleeSavedInfo();
  if (!CSI.empty()) {
    unsigned CalleeSavedFrameSizeByte = 0;
    const std::vector<CalleeSavedInfo> &CSI = MFI.getCalleeSavedInfo();

    for (const CalleeSavedInfo &I : CSI) {
      int64_t Offset = MFI.getObjectOffset(I.getFrameIdx());
      Register Reg = I.getReg();
      unsigned DReg = RegInfo.getDwarfRegNum(Reg, true);
      BuildCFI(MBB, MBBI, DL, TII, MCCFIInstruction::createOffset(nullptr, DReg, Offset));
      CalleeSavedFrameSizeByte += 2; // 2bytes = 16bits.
    }
    FuncInfo->setCalleeSavedFrameSize(CalleeSavedFrameSizeByte);
  }

  // Adjust stack.
  adjustStackPtr(MF, MBB, MBBI, StackSize);

  // TODO: in the future, check Alignment & Realign, if needed.

}

/* 
  emitEpilogue()

  callee:
    ...
    <callee job>
    SP <- FP
    pop FP  (assumed it was done...<---Question)
*/
void LC3FrameLowering::emitEpilogue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {

  const LC3Subtarget &STI = MF.getSubtarget<LC3Subtarget>();
  const LC3InstrInfo &TII = *STI.getInstrInfo();
  const LC3RegisterInfo &RegInfo = *STI.getRegisterInfo();
  
  LC3MachineFunctionInfo *FuncInfo = MF.getInfo<LC3MachineFunctionInfo>();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc DL = (MBBI != MBB.end()) ? MBBI->getDebugLoc() : DebugLoc();

  if (!hasFP(MF) || (MFI.getStackSize() == 0 && !MFI.adjustsStack()))
    return;

  // in emitPrologue(), we did "FP := SP". Now Write back to SP.
  BuildMI(MBB, MBBI, DL, TII.get(LC3::ADDrr), LC3::SP)
    .addReg(LC3::FP).setMIFlag(MachineInstr::FrameSetup);
}

void LC3FrameLowering::BuildCFI(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MBBI,
                                 const DebugLoc &DL,
                                 const LC3InstrInfo &TII,
                                 const MCCFIInstruction &CFIInst) const {

  MachineFunction &MF = *MBB.getParent();
  unsigned CFIIndex = MF.addFrameInst(CFIInst);
  BuildMI(MBB, MBBI, DL, TII.get(TargetOpcode::CFI_INSTRUCTION))
      .addCFIIndex(CFIIndex);
}

/* 
  hasFP - Return true if the specified function should have a dedicated frame pointer register.  
  Return true 
    if the function has variable sized allocas,
                    has input arguments <--------- TODO
*/
bool LC3FrameLowering::hasFP(const MachineFunction &MF) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  const LC3MachineFunctionInfo *FuncInfo = MF.getInfo<LC3MachineFunctionInfo>();

  return (MFI.hasVarSizedObjects());
}

// Adjsut StackPointer by Amount bytes.
void LC3FrameLowering::adjustStackPtr(MachineFunction &MF,
                                        MachineBasicBlock &MBB,
                                        MachineBasicBlock::iterator MBBI,
                                        uint16_t Amount
                                        ) const {
  // BuildMI(MachineBasicBlock, MachineInstr, DebugLoc/PCSection, MCInstDesc, DstReg).addReg().addReg/Imm()
  
  DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  const LC3Subtarget &STI = MF.getSubtarget<LC3Subtarget>();
  const LC3InstrInfo &TII = *STI.getInstrInfo();

  if (isInt<16>(Amount)) // ADD SP, SP, Imm5
  {
    BuildMI(MBB, MBBI, DL, TII.get(LC3::ADDri), LC3::SP).addReg(LC3::SP).addImm(Amount);
  }
  else { // ADD SP, SP, Reg
    Register Reg = TII.loadImmediate(MBB, MBBI, DL, Amount);
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