//===-- LC3argetObjectFile.h - LC3 Object Info -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_LC3_LC3TARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_LC3_LC3TARGETOBJECTFILE_H

#include "LC3TargetMachine.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {

class MCContext;
class TargetMachine;

class LC3TargetObjectFile : public TargetLoweringObjectFileELF {
    
    MCSection *SmallDataSection;
    MCSection *SmallBSSSection;
    // const LC3TargetMachine *TM;
public:
  LC3TargetObjectFile() = default;

  void Initialize(MCContext &Ctx, const TargetMachine &TM) override;

};

} // end namespace llvm

#endif
