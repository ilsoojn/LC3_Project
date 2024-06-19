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
#include "LC3.h"
#include "LC3MachineFunction.h"
#include "LC3RegisterInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
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

/// getGlobalBaseReg - Output the instructions required to put the
/// GOT address into a register.
SDNode *LC3DAGToDAGISel::getGlobalBaseReg() {
  Register GlobalBaseReg = MF->getInfo<LC3FunctionInfo>()->getGlobalBaseReg(*MF);
  return CurDAG->getRegister(GlobalBaseReg, 
                              getTargetLowering()->getPointerTy(CurDAG->getDataLayout()))
      .getNode();
}

bool SelectAddrRegImm(SDValue Addr, SDValue &Base, SDValue &Offset){}

bool SelectAddrRegReg(SDValue Addr, SDValue &Reg1, SDValue &Reg2){}

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

  // See if subclasses can handle this node.
  if (trySelect(Node))
    return;

  switch(Opcode) {
  default: break;

  case ISD::ADD:
    if (Node->getSimpleValueType(0).isVector() &&
        selectVecAddAsVecSubIfProfitable(Node))
      return;
    break;

  // Get target GOT address.
  case ISD::GLOBAL_OFFSET_TABLE:
    ReplaceNode(Node, getGlobalBaseReg());
    return;

#ifndef NDEBUG
  case ISD::LOAD:
  case ISD::STORE:
    assert((Subtarget->systemSupportsUnalignedAccess() ||
            cast<MemSDNode>(Node)->getAlign() >=
                cast<MemSDNode>(Node)->getMemoryVT().getStoreSize()) &&
           "Unexpected unaligned loads/stores.");
    break;
#endif
  }

  // Select the default instruction
  SelectCode(Node);
}

// bool LC3DAGToDAGISel::SelectInlineAsmMemoryOperand(
//     const SDValue &Op, InlineAsm::ConstraintCode ConstraintID,
//     std::vector<SDValue> &OutOps) {
//   // All memory constraints can at least accept raw pointers.
//   switch(ConstraintID) {
//   default:
//     llvm_unreachable("Unexpected asm memory constraint");
//   case InlineAsm::ConstraintCode::m:
//   case InlineAsm::ConstraintCode::R:
//   case InlineAsm::ConstraintCode::ZC:
//     OutOps.push_back(Op);
//     return false;
//   }
//   return true;
// }

char LC3DAGToDAGISel::ID = 0;

INITIALIZE_PASS(LC3DAGToDAGISel, DEBUG_TYPE, PASS_NAME, false, false)
