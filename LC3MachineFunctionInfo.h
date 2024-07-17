//=- LC3MachineFunctionInfo.h - LC3 machine function info ----*- C++ -*-=//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares LC3-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_LC3_LC3MACHINEFUNCTIONINFO_H
#define LLVM_LIB_TARGET_LC3_LC3MACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFunction.h"


namespace llvm {

  class LC3MachineFunctionInfo : public MachineFunctionInfo {
    virtual void anchor();
  private:
    Register GlobalBaseReg;

    /* 
      Below is from X86
        "many x86 calling conventions pass arguments on the stack. 
          The return value (or a pointer to it) is returned in a register." - Wikipedia Calling_convention
    */
    /// CalleeSavedFrameSize - Size of the callee-saved register portion of the
    /// stack frame in bytes.
    unsigned CalleeSavedFrameSize = 0;

    /// VarArgsFrameOffset - Frame offset to start of varargs area.
    int VarArgsFrameOffset;

  public:
    LC3MachineFunctionInfo()
      : GlobalBaseReg(0), CalleeSavedFrameSize(0), VarArgsFrameOffset(0) {}
    LC3MachineFunctionInfo(const Function &F, const TargetSubtargetInfo *STI)
        : GlobalBaseReg(0), CalleeSavedFrameSize(0), VarArgsFrameOffset(0) {}

    MachineFunctionInfo *
    clone(BumpPtrAllocator &Allocator, MachineFunction &DestMF,
          const DenseMap<MachineBasicBlock *, MachineBasicBlock *> &Src2DstMBB)
        const override;

    Register getGlobalBaseReg() const { return GlobalBaseReg; }
    void setGlobalBaseReg(Register Reg) { GlobalBaseReg = Reg; }

    int getVarArgsFrameOffset() const { return VarArgsFrameOffset; }
    void setVarArgsFrameOffset(int Offset) { VarArgsFrameOffset = Offset; }

    unsigned getCalleeSavedFrameSize() const { return CalleeSavedFrameSize; }
    void setCalleeSavedFrameSize(unsigned bytes) { CalleeSavedFrameSize = bytes; }

  };
  
}

#endif