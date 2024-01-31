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

#include "LC3RegisterInfo.h"
#include "LC3MachineFunctionInfo.h"
#include "LC3Subtarget.h"
#include "LC3.h"

#include "llvm/ADT/BitVector.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"

#define GET_REGINFO_TARGET_DESC
#include "LC3GenRegisterInfo.inc"

using namespace llvm;

#define DEBUG_TYPE "reginfo"

// <target>GenRegisterInfo("<target>::ReturnAddress/LinkRegister"){}LC3RegisterInfo::
LC3RegisterInfo::LC3RegisterInfo() : LC3GenRegisterInfo(LC3::LR) {}

//===----------------------------------------------------------------------===//
// Callee Saved Registers
//===----------------------------------------------------------------------===//
// In CallConv.td, def CSR : CalleeSavedRegs<(add SP)>
// Then, llc creates CSR_SaveList and CSR_RegMask
const MCPhysReg *
LC3RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const 
{
    // return CSR_SaveList;
    return nullptr;
}

const uint32_t *
LC3RegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                    CallingConv::ID CC) const 
{
    // return CSR_RegMask;
    return nullptr;
}

const TargetRegisterClass *LC3RegisterInfo::
getPointerRegClass(const MachineFunction &MF, unsigned Kind) const 
{
    return &LC3::GPRRegClass;
}

BitVector LC3RegisterInfo::
getReservedRegs(const MachineFunction &MF) const 
{
    // Initial Reserved CPU Registers (Status Register & PCounter)
    static const uint16_t ReservedCPURegs[] = {LC3::SR, LC3::PC};
    BitVector Reserved(getNumRegs());
    
    Reserved.set(ReservedCPURegs[0]);
    Reserved.set(ReservedCPURegs[1]);

    return Reserved;
}

/// Stack Frame Processing Methods
bool LC3RegisterInfo::
eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj, 
                    unsigned FIOperandNum, RegScavenger *RS) const 
{
    assert(SPAdj == 0 && "Unexpected");
    
    MachineInstr &MI = *II;
    DebugLoc DL = MI.getDebugLoc();
    MachineFunction &MF = *MI.getParent()->getParent();

    LLVM_DEBUG(dbgs() << "\nFunction : " << MF.getFunction().getName() << "\n" << MI);

    Register FrameReg;
    uint16_t FrameOffset;
    uint16_t FrameIndex = MI.getOperand(FIOperandNum).getIndex();
    uint16_t StackSize = MF.getFrameInfo().getStackSize();
    uint16_t StackOffset = MF.getFrameInfo().getObjectOffset(FrameIndex);

    /***********************************************************************
    * FIXME: the Load/Store and Goto/Return ISA of LC3 uses PC as a "base"
    ***********************************************************************/
    // TODO: Implement for FrameIndex of Dynamically_Allocated_Stack
    // Assumed Static Allocation for All
    FrameReg = LC3::SP;
    FrameOffset = StackOffset + StackSize;
    
    LLVM_DEBUG(dbgs() << "FrameIndex : " << FrameIndex << "\n"
                      << "StackSize  : " << StackSize << "\n"
                      << "StackOffset: " << StackOffset << "\n"
                      << "FrameOffset: " << FrameOffset << "\n");

    FrameOffset += MI.getOperand(FIOperandNum).getImm();

    MI.getOperand(FIOperandNum).ChangeToRegister(FrameReg, false);
    MI.getOperand(FIOperandNum+1).ChangeToImmediate(FrameOffset);
    
    return false;
    /**********************************************************************/
    
}

/// Debug information queries.
Register LC3RegisterInfo::
getFrameRegister(const MachineFunction &MF) const 
{
    const TargetFrameLowering *TFI = MF.getSubtarget().getFrameLowering();
    return TFI->hasFP(MF) ? (LC3::FP) : (LC3::SP);
}

using namespace llvm;
