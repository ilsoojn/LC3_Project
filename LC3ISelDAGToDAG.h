//===---- LC3ISelDAGToDAG.h - A Dag to Dag Inst Selector for LC3 --------===//
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

#ifndef LLVM_LIB_TARGET_LC3_LC3ISELDAGTODAG_H
#define LLVM_LIB_TARGET_LC3_LC3ISELDAGTODAG_H

#include "LC3.h"
#include "LC3Subtarget.h"
#include "LC3TargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"

//===----------------------------------------------------------------------===//
// Instruction Selector Implementation
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// LC3DAGToDAGISel - LC3 specific code to select LC3 machine
// instructions for SelectionDAG operations.
//===----------------------------------------------------------------------===//

namespace llvm {

class LC3DAGToDAGISel : public SelectionDAGISel {
public:
  static char ID;

  LC3DAGToDAGISel() = delete;

  explicit LC3DAGToDAGISel(LC3TargetMachine &TM)
      : SelectionDAGISel(ID, TM, OL), Subtarget(nullptr) {}

  bool runOnMachineFunction(MachineFunction &MF) override;

protected:
  SDNode *getGlobalBaseReg();
  /// Keep a pointer to the LC3Subtarget around so that we can make the right
  /// decision when generating code for different targets.
  const LC3Subtarget *Subtarget;

private:
  // Include the pieces autogenerated from the target description.
  #include "LC3GenDAGISel.inc"

  /// (reg + imm)
  virtual bool SelectAddrRegImm(SDValue Addr, SDValue &Base,SDValue &Offset) override;

  /// (reg + reg)
  virtual bool SelectAddrRegReg(SDValue Addr, SDValue &R1, SDValue &R2) override;

  void Select(SDNode *N) override;
  virtual bool trySelect(SDNode *Node) override;

  // getImm - Return a target constant with the specified value.
  inline SDValue getImm(const SDNode *Node, uint64_t Imm) {
    return CurDAG->getTargetConstant(Imm, SDLoc(Node), Node->getValueType(0));
  }

  // bool SelectInlineAsmMemoryOperand(const SDValue &Op,
  //                                   InlineAsm::ConstraintCode ConstraintID,
  //                                   std::vector<SDValue> &OutOps) override;
};

FunctionPass *createLC3ISelDag(Cpu0TargetMachine &TM){
  return new LC3DAGToDAGISel(TM);
}

}

#endif
