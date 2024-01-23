//===-- LC3TargetMachine.h - Define TargetMachine for LC3 ---*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the LC3 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_LC3_LC3TARGETMACHINE_H
#define LLVM_LIB_TARGET_LC3_LC3TARGETMACHINE_H

#include "LC3.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Target/TargetMachine.h"
#include <optional>


namespace llvm {

class LC3TargetMachine : public LLVMTargetMachine {

  virtual void anchor();
  std::unique_ptr<TargetLoweringObjectFile> TLOF;

public:
  /* 
  * This is bit different than "Writing an LLVM Backend" guide
  * GUIDE: XXXTargetmachine(const Module &M, const std::string &FS)
  *         : DataLayout("..."), Subtarget(M, FS), InstrInfo(Subtarget), FrameInfo(...) { ...... }
  * Below is from RISCVTargetMachine.h which follows LLVMTargetMachine() constructor format
  */
  LC3TargetMachine(const Target &T, 
                    /* DataLayoutString:computeDataLayout(TT) */
                    const Triple &TT, 
                    StringRef CPU,
                    StringRef FS, 
                    const TargetOptions &Options,
                    std::optional<Reloc::Model> RM, // RM:getEffectiveRelocModel(TT, RM)
                    std::optional<CodeModel::Model> CM, // CM:getEffectiveCodeModel(CM, Default:CodeModel::Small), OL)
                    CodeGenOptLevel OL,
                    bool JIT);
  ~LC3TargetMachine() override;

  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  // MachineFunctionInfo *
  // createMachineFunctionInfo(BumpPtrAllocator &Allocator, const Function &F,
  //                           const TargetSubtargetInfo *STI) const override;

}; // end TargetMachine class
} // end namespace llvm

#endif