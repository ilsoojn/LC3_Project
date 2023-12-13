//===-- LC3RegisterInfo.cpp - LC3 Register Information ----------------===//
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

#define DEBUG_TYPE "lc3-reginfo"

#include "LC3RegisterInfo.h"
#include "LC3.h"
//#include "LC3MachineFunctionInfo.h"
//#include "LC3Subtarget.h"

#include "llvm/IR/Type.h"

#include "llvm/ADT/BitVector.h"

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#define GET_REGINFO_TARGET_DESC
#include "LC3GenRegisterInfo.inc"

using namespace llvm;

// <target>GenRegisterInfo("<target>::ReturnAddress/LinkRegister"){}LC3RegisterInfo::
LC3RegisterInfo::LC3RegisterInfo() : LC3GenRegisterInfo(LC3::R7) {}

//===----------------------------------------------------------------------===//
// Callee Saved Registers
//===----------------------------------------------------------------------===//
// In Cpu0CallConv.td, def CSR : CalleeSavedRegs<(add R6)>
// llc creates CSR_SaveList and CSR_RegMask
// const MCPhysReg *
// LC3RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const 
// {
//     return CSR_SaveList;
// }

// const uint32_t *
// LC3RegisterInfo::getCallPreservedMask(const MachineFunction &MF,
//                                     CallingConv::ID CC) const 
// {
//     return CSR_RegMask;
// }

const TargetRegisterClass *LC3RegisterInfo::
getPointerRegClass(const MachineFunction &MF, unsigned Kind) const 
{
}

BitVector LC3RegisterInfo::
getReservedRegs(const MachineFunction &MF) const 
{
    BitVector Reserved(getNumRegs());

    Reserved.set( {LC3::R5, LC3::R6, LC3::R7} );
    return Reserved;
}

/// Stack Frame Processing Methods
bool LC3RegisterInfo::
eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj, 
                    unsigned FIOperandNum, RegScavenger *RS) const 
{
}

/// Debug information queries.
Register LC3RegisterInfo::
getFrameRegister(const MachineFunction &MF) const 
{
}

using namespace llvm;
