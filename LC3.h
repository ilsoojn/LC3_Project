//===-- LC3.h - Top-level interface for LC3 representation --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// LC3 back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_LC3_LC3_H
#define LLVM_LIB_TARGET_LC3_LC3_H

#include "MCTargetDesc/LC3MCTargetDesc.h"
#include "llvm/PassRegistry.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
    class LC3TargetMachine;
    class LC3Subtarget;
    class FunctionPass;
    class MachineInst;
    class PassRegistry;

    FunctionPass *createLC3ISelDag(LC3TargetMachine &TM);
    void initializeLC3DAGToDAGISelPass(PassRegistry &PR);

}  // end namespace llvm

namespace llvm {

} // end namespace llvm

#endif
