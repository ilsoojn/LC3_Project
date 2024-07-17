//===-- LC3ISelDAGToDAG.cpp - A Dag to Dag Inst Selector for LC3 --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the LC3 target.
//
//===----------------------------------------------------------------------===//

#include "LC3ISelDAGToDAG.h"

using namespace llvm;

#define DEBUG_TYPE "lc3-isel"
#define PASS_NAME "LC3 DAG->DAG Pattern Instruction Selection"

//===----------------------------------------------------------------------===//
// Instruction Selector Implementation
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// LC3DAGToDAGISel - LC3 specific code to select LC3 machine
// instructions for SelectionDAG operations.
//===----------------------------------------------------------------------===//

bool LC3DAGToDAGISel::runOnMachineFunction(MachineFunction &MF) {
  Subtarget = &MF.getSubtarget<LC3Subtarget>();
  return SelectionDAGISel::runOnMachineFunction(MF);;
}

bool LC3DAGToDAGISel::SelectAddr(SDValue Addr, SDValue &Offset){

  LLVM_DEBUG(dbgs() << "SelectAddr:\n\t");
  Addr.dump();

  // TODO: have its own RATHER than calling the SelectAddrRegImm
  SDValue Base;
  return SelectAddrRegImm(Addr, Base, Offset);
}

bool LC3DAGToDAGISel::SelectAddrRegImm(SDValue Addr, SDValue &Base, SDValue &Offset){

  LLVM_DEBUG(dbgs() << "SelectAddrRegImm:\n\t");
  Addr.dump();

  DataLayout DL = CurDAG->getDataLayout();
  MVT PtrVT = TLI->getPointerTy(DL);
  MVT ImmVT = Addr.getSimpleValueType();

  // FrameIndex + Imm
  if (FrameIndexSDNode *FrameIdxNode = dyn_cast<FrameIndexSDNode>(Addr)) {
    Base = CurDAG->getTargetFrameIndex(FrameIdxNode->getIndex(), PtrVT);
    Offset = CurDAG->getTargetConstant(0, SDLoc(Addr), ImmVT);
    return true;
  }

  // Question: So many <TARGET>ISelDAGToDAG checks for FrameIdxSDNode, first as above
  //           Then Doit as below again,.... Why is that?
  if (!CurDAG->isBaseWithConstantOffset(Addr))
    return false;

  // <Reg + Imm> Pattern
  if (ConstantSDNode *ConstNode = dyn_cast<ConstantSDNode>(Addr.getOperand(1))) {
    if (!isInt<3>(ConstNode->getSExtValue())) {
      Base = Addr.getOperand(0);
      if (FrameIndexSDNode *FrameIdxNode = dyn_cast<FrameIndexSDNode>(Addr.getOperand(0)))
        Base = CurDAG->getTargetFrameIndex(FrameIdxNode->getIndex(), PtrVT);
      
      Offset = CurDAG->getTargetConstant(ConstNode->getZExtValue(), SDLoc(Addr), ImmVT);
      return true;
    }
  }
  Base = Addr;
  Offset = CurDAG->getTargetConstant(0, SDLoc(Addr), ImmVT);
  return true;
}

bool LC3DAGToDAGISel::SelectAddrRegReg(SDValue Addr, SDValue &Reg1, SDValue &Reg2){
  
  LLVM_DEBUG(dbgs() << "SelectAddrRegReg:\n\t");
  Addr.dump();

  DataLayout DL = CurDAG->getDataLayout();
  MVT PtrVT = TLI->getPointerTy(DL);

  // FrameIndex + Register ???
  if (Addr.getOpcode() == ISD::FrameIndex) return false;

  if (Addr.getOpcode() == ISD::ADD) {
    if (ConstantSDNode *ConstNode = dyn_cast<ConstantSDNode>(Addr.getOperand(1)))
      // <Reg + Reg> Pattern
      if (isInt<3>(ConstNode->getSExtValue())){
        Reg1 = Addr.getOperand(0);
        Reg2 = Addr.getOperand(1);
        return true;
      }
      return false; // Possibly <Reg + Imm> Pattern
  }

  Reg1 = Addr;
  Reg2 = CurDAG->getRegister(LC3::R0, PtrVT);
  return true;

}

/// Select instructions not customized! Used for
/// expanded, promoted and normal instructions
void LC3DAGToDAGISel::Select(SDNode *Node) {
  unsigned Opcode = Node->getOpcode();

  // If we have a custom node, we already have selected!
  if (Node->isMachineOpcode()) {
    LLVM_DEBUG(errs() << "== "; Node->dump(CurDAG); errs() << "\n");
    Node->setNodeId(-1);
    return;
  }

  // TODO: do I want to have 'select<ISD::ISDOP>' for special OP first (eg LD/ST/FrameIndex...)
  /*
  switch (N->getOpcode()) {
  default: break;
  case ISDE::LOAD: return select<ISD::LOAD>(Node);
  ...
  }
  */
  
  // Select the default instruction
  SelectCode(Node);
}

char LC3DAGToDAGISel::ID = 0;

INITIALIZE_PASS(LC3DAGToDAGISel, DEBUG_TYPE, PASS_NAME, false, false)
