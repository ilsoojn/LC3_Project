//===-- LC3FrameLowering.h - Define frame lowering for LC3 --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_LC3_LC3FRAMELOWERING_H
#define LLVM_LIB_TARGET_LC3_LC3FRAMELOWERING_H

#include "LC3.h"
#include "LC3InstrInfo.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/Support/TypeSize.h"

namespace llvm {

class LC3Subtarget;
class LC3InstrInfo;

class LC3FrameLowering : public TargetFrameLowering {
  
public:
  explicit LC3FrameLowering(const LC3Subtarget &STI);

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  // bool hasReservedCallFrame(const MachineFunction &MF) const override;
  // void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
  //                           RegScavenger *RS = nullptr) const override;
  // bool restoreCalleeSavedRegisters(MachineBasicBlock &MBB,
  //                             MachineBasicBlock::iterator MI,
  //                             MutableArrayRef<CalleeSavedInfo> CSI,

  void BuildCFI(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI,
                const DebugLoc &DL, const LC3InstrInfo &TII, const MCCFIInstruction &CFIInst) const;

  /// hasFP - Return true if the specified function should have a dedicated
  /// frame pointer register. For most targets this is true only if the function
  /// has variable sized allocas or if frame pointer elimination is disabled.
  bool hasFP(const MachineFunction &MF) const override;
  
  MachineBasicBlock::iterator
  eliminateCallFramePseudoInstr(MachineFunction &MF,
                                MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator I) const override;

  void adjustStackPtr(MachineFunction &MF,
                        MachineBasicBlock &MBB,
                        MachineBasicBlock::iterator MBBI,
                        uint16_t Amount) const;
};

} // End llvm namespace

#endif
