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
#include "LC3Subtarget.h"
#include "LC3MachineFunctionInfo.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "instrinfo"

#define GET_INSTRINFO_CTOR_DTOR
#include "LC3GenInstrInfo.inc"

// Pin the vtable to this file.
void LC3InstrInfo::anchor() {}

LC3InstrInfo::LC3InstrInfo(LC3Subtarget &STI)
    : LC3GenInstrInfo(LC3::ADJCALLSTACKDOWN, LC3::ADJCALLSTACKUP),
      Subtarget(STI) {}

