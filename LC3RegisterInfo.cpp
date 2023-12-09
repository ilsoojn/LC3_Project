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
#include "LC3MachineFunctionInfo.h"
#include "LC3Subtarget.h"

#include "llvm/IR/Type.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#define GET_REGINFO_TARGET_DESC
#include "LC3GenRegisterInfo.inc"

using namespace llvm;
