//===-- LC3RegisterInfo.h - LC3 Register Information Impl ---*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the LC3 implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_LC3_LC3REGISTERINFO_H
#define LLVM_LIB_TARGET_LC3_LC3REGISTERINFO_H

#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "LC3GenRegisterInfo.inc"

namespace llvm {
struct LC3RegisterInfo : public LC3GenRegisterInfo {
  LC3RegisterInfo();

  /// Code Generation virtual methods...
  // const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;
  // const uint32_t *getCallPreservedMask(const MachineFunction &MF,
  //                                      CallingConv::ID CC) const override;

  const TargetRegisterClass *getPointerRegClass(const MachineFunction &MF,
                                                unsigned Kind) const override;

  BitVector getReservedRegs(const MachineFunction &MF) const override;

  /// Stack Frame Processing Methods
  bool eliminateFrameIndex(MachineBasicBlock::iterator II,
                           int SPAdj, unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;

  /// Debug information queries.
  Register getFrameRegister(const MachineFunction &MF) const override;

  /// Register scavenging; find unused ones
  /* TODO */
};

} // end namespace llvm

#endif
