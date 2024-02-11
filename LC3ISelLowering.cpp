//===-- LC3ISelLowering.cpp - LC3 DAG Lowering Implementation ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the interfaces that LC3 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

// #include "MCTargetDesc/LC3MCExpr.h"
#include "LC3ISelLowering.h"
#include "LC3MachineFunctionInfo.h"
#include "LC3RegisterInfo.h"
#include "LC3TargetMachine.h"
#include "LC3TargetObjectFile.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/KnownBits.h"

using namespace llvm;

#define DEBUG_TYPE "isellowering"

//===----------------------------------------------------------------------===//
// Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "LC3GenCallingConv.inc"

//===----------------------------------------------------------------------===//
// TargetLowering Implementation
//===----------------------------------------------------------------------===//

LC3TargetLowering::LC3TargetLowering(const TargetMachine &TM,
                                         const LC3Subtarget &STI)
    : TargetLowering(TM), Subtarget(&STI) {
  MVT PtrVT = MVT::getIntegerVT(TM.getPointerSizeInBits(0));
  // TODO
}

const char *LC3TargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch ((LC3ISD::NodeType)Opcode) {
  case LC3ISD::FIRST_NUMBER:    break;
  }
  return nullptr;
}

SDValue LC3TargetLowering::
LowerOperation(SDValue Op, SelectionDAG &DAG) const {

  
  // switch (Op.getOpcode()) {
  // default: llvm_unreachable("Should not custom lower this!");

  // //case ISD::SOMETHING:         return LowerSOMETHING(Op, DAG, *this, Subtarget);
  // }
}

//===----------------------------------------------------------------------===//
//                         LC3 Inline Assembly Support
//===----------------------------------------------------------------------===//

