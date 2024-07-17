//===-- LC3InstrInfo.cpp - LC3 Instruction Information ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the LC3 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "LC3.h"
#include "LC3InstrInfo.h"
#include "LC3RegisterInfo.h"
#include "LC3Subtarget.h"
#include "LC3MachineFunctionInfo.h"
#include "MCTargetDesc/LC3MCTargetDesc.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"
#include <cstdint>

using namespace llvm;

#define DEBUG_TYPE "instrinfo"

#define GET_INSTRINFO_CTOR_DTOR
#include "LC3GenInstrInfo.inc"

// Pin the vtable to this file.
void LC3InstrInfo::anchor() {}

LC3InstrInfo::LC3InstrInfo(LC3Subtarget &STI)
  : LC3GenInstrInfo(LC3::ADJCALLSTACKDOWN, LC3::ADJCALLSTACKUP),
    Subtarget(STI) {}

unsigned LC3InstrInfo::isLoadFromStackSlot(const MachineInstr &MI, int &FrameIndex) const {
  switch(MI.getOpcode()){
    case LC3::LD:
    case LC3::LDR:
      if(MI.getOperand(1).isFI() && 
         MI.getOperand(2).isImm() && MI.getOperand(2).getImm() == 0)
      {
        FrameIndex = MI.getOperand(1).getIndex();
        return MI.getOperand(0).getReg();
      }
      break;
    default:
      break;
  }

  return 0;
}

unsigned LC3InstrInfo::isStoreToStackSlot(const MachineInstr &MI, int &FrameIndex) const {
  switch(MI.getOpcode()){
    case LC3::ST:
    case LC3::STR:
      if(MI.getOperand(1).isFI() && 
         MI.getOperand(2).isImm() && MI.getOperand(2).getImm() == 0)
      {
        FrameIndex = MI.getOperand(1).getIndex();
        return MI.getOperand(0).getReg();
      }
      break;
    default:
      break;
  }

  return 0;
}

// copyPhysReg() - typically for MOV instruction (not available in LC3)
//                 ADD DestReg SrcReg #0
void LC3InstrInfo::copyPhysReg(MachineBasicBlock &MBB, 
                               MachineBasicBlock::iterator I,
                               const DebugLoc &DL, MCRegister DestReg, 
                               MCRegister SrcReg, bool KillSrc) const {
  
  // Copy To/From Register
  if(LC3::GPRRegClass.contains(DestReg, SrcReg)){
    BuildMI(MBB, I, DL, get(LC3::ADDri)).addReg(DestReg).addReg(SrcReg).addImm(0);
    return;
  }
  
  // assert(opcode && "Cannot copy registers");
  llvm_unreachable("Cannot copy register");
}

void LC3InstrInfo::storeRegToStack(MachineBasicBlock &MBB, MachineBasicBlock::iterator I, 
                        Register SrcReg, bool isKill, int FrameIndex,
                        const TargetRegisterClass *RC, const TargetRegisterInfo *TRI, 
                        int16_t Offset) const {

  DebugLoc DL;
  if (I != MBB.end()) DL = I->getDebugLoc();

  MachineFunction *MF = MBB.getParent();
  const MachineFrameInfo &MFI = MF->getFrameInfo();
  MachineMemOperand *MMO = MF->getMachineMemOperand(MachinePointerInfo::getFixedStack(*MF, FrameIndex), 
                                                  MachineMemOperand::MOStore, 
                                                  MFI.getObjectSize(FrameIndex), 
                                                  MFI.getObjectAlign(FrameIndex));

  unsigned opcode = 0;
  if(TRI->isTypeLegalForClass(*RC, MVT::i16))
    opcode = LC3::ST; // when is LC3::STI
  // else 
  //  llvm_unreachable("Can't load this register to stack slot");
    
  assert(opcode && "Invalid Opcode for StoreRegfromStackSlot");
    
  BuildMI(MBB, I, DL, get(opcode))
    .addFrameIndex(FrameIndex)
    .addImm(Offset)
    .addReg(SrcReg, getKillRegState(isKill))
    .addMemOperand(MMO);

}

void LC3InstrInfo::loadRegFromStack(MachineBasicBlock &MBB, MachineBasicBlock::iterator I, 
                        Register DestReg, int FrameIndex, 
                        const TargetRegisterClass *RC, const TargetRegisterInfo *TRI,
                        int16_t Offset) const {
  DebugLoc DL;
  if (I != MBB.end()) DL = I->getDebugLoc();

  MachineFunction *MF = MBB.getParent();
  const MachineFrameInfo &MFI = MF->getFrameInfo();
  MachineMemOperand *MMO = MF->getMachineMemOperand(MachinePointerInfo::getFixedStack(*MF, FrameIndex), 
                                                  MachineMemOperand::MOLoad, 
                                                  MFI.getObjectSize(FrameIndex), 
                                                  MFI.getObjectAlign(FrameIndex));

  unsigned opcode = 0;
  if(TRI->isTypeLegalForClass(*RC, MVT::i16))
    opcode = LC3::LD; // when is LC3::LDI ?
  // else 
  //  llvm_unreachable("Can't load this register to stack slot");
    
  assert(opcode && "Invalid Opcode for LoadRegfromStackSlot");

  BuildMI(MBB, I, DL, get(opcode), DestReg)
    .addFrameIndex(FrameIndex)
    .addImm(Offset)
    .addMemOperand(MMO);
}

unsigned LC3InstrInfo::getInstSizeInBytes(const MachineInstr &MI) const {

  if (MI.isInlineAsm()) { // TargetOpcode::INLINEASM
    const MachineFunction *MF = MI.getParent()->getParent();
    const char *AsmStr = MI.getOperand(0).getSymbolName();
    return getInlineAsmLength(AsmStr, *MF->getTarget().getMCAsmInfo());
  }

  return MI.getDesc().getSize();
}

// This is idea similar from the Cpu0SEInstrInfo.cpp
// absent of MOV, we (repeatly) ADD R4 R4 #imm5 
Register LC3InstrInfo::loadImmediate(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                                    const DebugLoc &DL, int16_t Imm) const {
  if(!isInt<16>(Imm))
    report_fatal_error("Can't loadImmediate. Outside of the signed 16-bit range not supported");
  const TargetRegisterInfo *TRI = &getRegisterInfo();
  
  // FIXME: Assumed all registers are alive; thus, save R4 to use
  Register TmpReg = LC3::R4;
  storeRegToStack(MBB, I, TmpReg, false, LC3::SP, &LC3::GPRRegClass, TRI, 0);
  BuildMI(MBB, I, DL, get(LC3::ANDri))
      .addReg(TmpReg)
      .addImm(0);
  
  // R4 <- Imm
  while(!isIntN(5, Imm)){
    Imm -= 32;
    BuildMI(MBB, I, DL, get(LC3::ADDri), TmpReg)
      .addReg(TmpReg)
      .addImm(32);
  }// while
  BuildMI(MBB, I, DL, get(LC3::ADDri), TmpReg)
      .addReg(TmpReg)
      .addImm(Imm);

  return LC3::R4;
}


//===----------------------------------------------------------------------===//
//                          BRANCH RELATED FUNCTIONS                          //
//===----------------------------------------------------------------------===//

LC3CC::CondCodes LC3InstrInfo::getBrCond(MachineOperand BrCondCode) const {

  switch(BrCondCode.getImm())
  {
    default: 
      return LC3CC::BINVALID;
    case 0:
      return LC3CC::BUNCOND;
    case 1:
      return LC3CC::BG;
    case 2:
      return LC3CC::BEQ;
    case 3:
      return LC3CC::BGE;
    case 4:
      return LC3CC::BL;
    case 5:
      return LC3CC::BNE;
    case 6:
      return LC3CC::BLE;
    case 7:
      return LC3CC::BUNCOND;
  }
  llvm_unreachable("Invalide branch instruction!");
}

LC3CC::CondCodes LC3InstrInfo::getOppositeBranchCondition(LC3CC::CondCodes CC) const {
  switch(CC){
    default:
      llvm_unreachable("Invalid Condition!");
  #if NZP
    case LC3CC::BP:
      return LC3CC::BZP;
    case LC3CC::BZ:
      return LC3CC::BNP;
    case LC3CC::BN:
      return LC3CC::BNZ;
    case LC3CC::BZP:
      return LC3CC::BN;
    case LC3CC::BNP:
      return LC3CC::BZ;
    case LC3CC::BNZ:
      return LC3CC::BP;
    case LC3CC::BNZP:
      return LC3CC::BNZP;
  #else
    case LC3CC::BEQ:
      return LC3CC::BNE;
    case LC3CC::BNE:
      return LC3CC::BEQ;
    case LC3CC::BL:
      return LC3CC::BGE;
    case LC3CC::BGE:
      return LC3CC::BL;
    case LC3CC::BG:
      return LC3CC::BLE;
    case LC3CC::BLE:
      return LC3CC::BG;
    case LC3CC::BUNCOND:
      return LC3CC::BUNCOND;
  #endif
  }
}

bool LC3InstrInfo::reverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const {

  assert(Cond.size() == 1 && "Invalid Branch Condition to reverse");
  LC3CC::CondCodes CC = static_cast<LC3CC::CondCodes>(Cond[0].getImm());

  Cond[0].setImm(getOppositeBranchCondition(CC));

  return false;
}

bool LC3InstrInfo::isBranchOffsetInRange(unsigned BranchOp, int64_t Offset) const {
  assert(isIntN(16, Offset) && "Branching offset is out of range.");
    
  switch(BranchOp) {
    default:
      llvm_unreachable("Unexpected Offset Branch opcode!");
    case LC3::JSR:
      return isIntN(11, Offset);
    case LC3::BR:
      return isIntN(9, Offset);
  }
}

MachineBasicBlock * LC3InstrInfo::getBranchDestBlock(const MachineInstr &MI) const {
  switch (MI.getOpcode()) {
  default:
    llvm_unreachable("Unexpected branch opcode!");
  case LC3::BR:
    return MI.getOperand(1).getMBB();
  case LC3::JSR:
  case LC3::JMP:
  case LC3::JSRR:
    return MI.getOperand(0).getMBB();
  }
}

// see llvm/include/llvm/CodeGen/targetInstrInfo.h::621 analyzeBranch() 
// for details about each case_#
bool LC3InstrInfo::analyzeBranch(MachineBasicBlock &MBB,
                                 MachineBasicBlock *&TBB,
                                 MachineBasicBlock *&FBB,
                                 SmallVectorImpl<MachineOperand> &Cond,
                                 bool AllowModify) const {

  MachineBasicBlock::iterator I = MBB.getLastNonDebugInstr();
  MachineBasicBlock::iterator FirstTerminator = MBB.getFirstTerminatorForward();

  // FIXME: Missing to handle when BB has 3 or more Terminator Instructions.
  // case 1) Don't know what to do when Termiantor is Not Branching Instruction.
  if(I->getDesc().isBranch()) 
    return true;
  if (I == MBB.end() || !isUnpredicatedTerminator(*I))
    return false;

  // case 2) UnConditional Branching
  if(FirstTerminator == I && I->getDesc().isUnconditionalBranch())
  {
    TBB = getBranchDestBlock(*I);
    // MBB_i: 
    //  INSTR1
    //  INSTR2
    //  ...
    //  JMP MBB_(i+1) <-------------- no use of this Unconditional Br, Erase it
    //
    // MBB_(i+1):
    //  INSTR1
    //  ...
    if(MBB.isLayoutSuccessor(TBB)){
      TBB = nullptr;
      I->eraseFromParent();
    }
    return false;
  }

  // case 3) Conditional Branching
  //         BR cc  TBB
  // 
  // MBB_FBB: ......
  if(FirstTerminator == I && I->getDesc().isConditionalBranch())
  {
    LC3CC::CondCodes CC = getBrCond(I->getOperand(0));
    if(CC == LC3CC::BINVALID) // Error in BR instruction
      return true;
    Cond.push_back(MachineOperand::CreateImm(CC));
    // Cond.push_back(I->getOperand(1));
    // Cond.push_back(I->getOperand(2));
    TBB = I->getOperand(1).getMBB();
    return false;
  }

  // case 4) Conditional Branching followed by an Unconditional one
  // TODO:
  //         BR cc  TBB   -----> BR !cc FBB    -----> BR !cc FBB 
  //         JMP    FBB   -----> JMP    TBB
  // 
  // MBB_TBB: ......
  if(FirstTerminator->getDesc().isConditionalBranch() && I->getDesc().isUnconditionalBranch())
  {
    TBB = getBranchDestBlock(*FirstTerminator);
    LC3CC::CondCodes CC = getBrCond(FirstTerminator->getOperand(0));
    if(CC == LC3CC::BINVALID) // Error in BR instruction
      return true;
    Cond.push_back(MachineOperand::CreateImm(CC));
    FBB = getBranchDestBlock(*I);

    return false;  
  }

  return true;
}

unsigned LC3InstrInfo::insertBranch(MachineBasicBlock &MBB,
                                    MachineBasicBlock *TBB,
                                    MachineBasicBlock *FBB,
                                    ArrayRef<MachineOperand> Cond,
                                    const DebugLoc &DL, int *BytesAdded) const {
  if (BytesAdded)
    *BytesAdded = 0;

  // Shouldn't be a fall through.
  assert(TBB && "insertBranch must not be told to insert a fallthrough");
  assert((Cond.size() == 1 || Cond.size() == 0) &&
         "LC3 branch conditions have one component!");

  // Unconditional branch.
  if (Cond.empty()) {
    assert(!FBB && "Unconditional branch with multiple successors!");
    auto &MI = *BuildMI(&MBB, DL, get(LC3::JMP)).addMBB(TBB);
    if (BytesAdded)
      *BytesAdded += getInstSizeInBytes(MI);
    return 1;
  }

  // Conditional branch.
  unsigned Count = 0;
  LC3CC::CondCodes CC = (LC3CC::CondCodes)Cond[0].getImm();
  auto &CondMI = *BuildMI(&MBB, DL, get(LC3::BR)).addImm(CC).addMBB(TBB);

  if (BytesAdded)
    *BytesAdded += getInstSizeInBytes(CondMI);
  ++Count;

  if (FBB) {
    // Two-way Conditional branch. Insert the second branch.
    auto &MI = *BuildMI(&MBB, DL, get(LC3::JMP)).addMBB(FBB);
    if (BytesAdded)
      *BytesAdded += getInstSizeInBytes(MI);
    ++Count;
  }

  return Count;
}

unsigned LC3InstrInfo::removeBranch(MachineBasicBlock &MBB,
                                    int *BytesRemoved) const {
  if (BytesRemoved)
    *BytesRemoved = 0;

  MachineBasicBlock::iterator I = MBB.end();
  unsigned Count = 0;

  while (I != MBB.begin()) {
    --I;
    if (I->isDebugInstr())
      continue;

    //: TODO: add here the missing jmp instructions once they are implemented
    // like jmp, {e}ijmp, and other cond branches, ...
    if (I->getOpcode() != LC3::JMP &&
        getBrCond(I->getOperand(0)) == LC3CC::BINVALID) 
      break;

    // Remove the branch.
    if (BytesRemoved)
      *BytesRemoved += getInstSizeInBytes(*I);
    I->eraseFromParent();
    I = MBB.end();
    ++Count;
  }

  return Count;
}
