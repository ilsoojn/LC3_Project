//===-- LC3ISelLowering.h - LC3 DAG Lowering Interface ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that LC3 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_LC3_LC3ISELLOWERING_H
#define LLVM_LIB_TARGET_LC3_LC3ISELLOWERING_H

#include "LC3.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {
  class LC3Subtarget;

  namespace LC3ISD {
  enum NodeType : unsigned {
    FIRST_NUMBER = ISD::BUILTIN_OP_END,
  };
  }

  class LC3TargetLowering : public TargetLowering {
    const LC3Subtarget *Subtarget;
  public:
    LC3TargetLowering(const TargetMachine &TM, const LC3Subtarget &STI);
    SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

    /// getTargetNodeName - This method returns the name of a target specific DAG node.    
    const char *getTargetNodeName(unsigned Opcode) const override;

  };
} // end namespace llvm

#endif // LLVM_LIB_TARGET_LC3_LC3ISELLOWERING_H
