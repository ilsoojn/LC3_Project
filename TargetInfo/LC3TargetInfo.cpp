//===-- LC3TargetInfo.cpp - LC3 Target Implementation -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/LC3TargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

Target &llvm::getTheLC3Target() {
  static Target TheLC3Target;
  return TheLC3Target;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeLC3TargetInfo() {
  RegisterTarget<Triple::lc3, /*HasJIT=*/false> X(
    getTheLC3Target(), "lc3", "Little Computer 3", "LC3");
}
