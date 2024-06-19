//===-- LC3TargetMachine.cpp - Define TargetMachine for LC3 -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#include "LC3TargetMachine.h"
#include "LC3.h"
#include "LC3MachineFunctionInfo.h"
#include "LC3TargetObjectFile.h"
#include "TargetInfo/LC3TargetInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

#define DEBUG_TYPE "targetmachine"

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeLC3Target() {
  // Register the target.
  RegisterTargetMachine<LC3TargetMachine> X(getTheLC3Target());\

//   PassRegistry &PR = *PassRegistry::getPassRegistry();
//   initializeLC3DAGToDAGISelPass(PR);
}

static StringRef computeDataLayout(const Triple &TT) {

   /**
   * # (n-bit) : # (alignment bit) : # (alwyas try to align)
   * eg) -v64:32:64-v128:32:128 (two sized vector : aligns 32-bit : always try to give them natural alignment )
   *
   * e/E       (Little / Big Endianness)
   * -m:e
   * -p:#:#    (Size of Pointer #-bit : aligned to #-bit)
   * [-Fi#]    (#-bit Function Pointer)
   * -i#:#     (#-bit Integer : aligned to #-bit)
   * [-f#:#:#] (#-bit Floating point : aligned to # bit : try to give align #-bit)
   * [-v#:]    (vector : APCS ABI align bit : trying align bit)
   * -n        (Integer Register)
   * -S        (Stack)
   **/

    if(TT.isArch16Bit())
        return "e-m:e-p:16:16-Fi16-i16:16-v16:16-n16-S16";

    assert(TT.isArch16Bit() && "only Arch 16-bit is supported");
//    return "e-m:e-p:32:32-i64:64-n32-S128";
    return "e-m:e-p:16:16-Fi16-i16:16-v16:16-n16-S16";
}

static Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM) {
    return RM.value_or(Reloc::Static);
}

LC3TargetMachine::LC3TargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       std::optional<Reloc::Model> RM,
                                       std::optional<CodeModel::Model> CM,
                                       CodeGenOptLevel OL, bool JIT)
    : LLVMTargetMachine(T, computeDataLayout(TT), TT, CPU, FS, Options,
                        getEffectiveRelocModel(RM),
                        getEffectiveCodeModel(CM, CodeModel::Small),OL),
      TLOF(std::make_unique<LC3TargetObjectFile>())
{
    initAsmInfo();
}

LC3TargetMachine::~LC3TargetMachine() = default;

void LC3TargetMachine::anchor() { }

const LC3Subtarget *
LC3TargetMachine::getSubtargetImpl(const Function &F) const {
  Attribute CPUAttr = F.getFnAttribute("target-cpu");
  Attribute FSAttr = F.getFnAttribute("target-features");

  std::string CPU =
      CPUAttr.isValid() ? CPUAttr.getValueAsString().str() : TargetCPU;
  std::string FS =
      FSAttr.isValid() ? FSAttr.getValueAsString().str() : TargetFS;

/* 
// Question: what is 'use-soft-float' attribute at line 95 ?
  // FIXME: This is related to the code below to reset the target options,
  // we need to know whether or not the soft float flag is set on the
  // function, so we can enable it as a subtarget feature.
  bool softFloat = F.getFnAttribute("use-soft-float").getValueAsBool();

  if (softFloat)
    FS += FS.empty() ? "+soft-float" : ",+soft-float";
// QuestionEnd
*/

  auto &I = SubtargetMap[CPU + FS];
  if (!I) {
    // This needs to be done before we create a new subtarget since any
    // creation will depend on the TM and the code generation flags on the
    // function that reside in TargetOptions.
    resetTargetOptions(F);
    I = std::make_unique<LC3Subtarget>(CPU, FS, *this);
  }
  return I.get();
}
/*
MachineFunctionInfo *LC3TargetMachine::createMachineFunctionInfo(
    BumpPtrAllocator &Allocator, const Function &F,
    const TargetSubtargetInfo *STI) const {
  return LC3MachineFunctionInfo::create<LC3MachineFunctionInfo>(Allocator,
                                                                    F, STI);
}
*/

namespace {
/// LC3 Code Generator Pass Configuration Options.
class LC3PassConfig : public TargetPassConfig {
public:
    LC3PassConfig(LC3TargetMachine &TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {}

    LC3TargetMachine &getLC3TargetMachine() const {
        return getTM<LC3TargetMachine>();
    }
    const LC3Subtarget &getLC3Subtarget() const {
    return *getLC3TargetMachine().getSubtargetImpl();
    }

    void addIRPasses() override;
    bool addInstSelector() override;
    void addPreEmitPass() override;
    // void addPreRegAlloc() override;
};
} // namespace

TargetPassConfig *LC3TargetMachine::createPassConfig(PassManagerBase &PM) {
    return new LC3PassConfig(*this, PM);
}

void LC3PassConfig::addIRPasses() {
    // addPass(createAtomicExpandPass());

    TargetPassConfig::addIRPasses();
}

bool LC3PassConfig::addInstSelector() {
    addPass(createLC3ISelDag(getLC3TargetMachine()/*getOptLevel()*/));
    return false;
}

void LC3PassConfig::addPreEmitPass(){
    // typicaly located in the target_header file, but also could be in a seprate header file.
    // addPass(new createLC3xxxPass()); 
}
