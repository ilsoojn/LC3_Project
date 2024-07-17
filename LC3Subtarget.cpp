//===-- LC3Subtarget.cpp - LC3 Subtarget Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the LC3 specific subclass of TargetSubtargetInfo.
//--------------------------------===//

#include "LC3Subtarget.h"
#include "LC3.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/MC/TargetRegistry.h"
//===--------------------------------------
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "LC3GenSubtargetInfo.inc"

void LC3Subtarget::anchor() { }

LC3Subtarget &LC3Subtarget::initializeSubtargetDependencies(StringRef CPU, StringRef FS) {

  if (TargetTriple.getArch() == Triple::lc3) {
    if (CPU.empty()){
      CPU = "LC3CPU";
    } else {
        errs() << "ERROR: TargetTriple.getArch() = " << TargetTriple.getArch() <<  " CPU = " << CPU << "\n";
        exit(0);
    }
  }

  // Parse features string.
  ParseSubtargetFeatures(CPU, /*TuneCPU*/ CPU, FS);

  return *this;
}

// LC3GenSubtargetInfo will display features by llc -march=LC3 -mcpu=help
LC3Subtarget::LC3Subtarget(const StringRef &CPU, const StringRef &FS, const TargetMachine &TM)
    :   LC3GenSubtargetInfo(TM.getTargetTriple(), CPU, /*TuneCPU*/ CPU, FS),
        TargetTriple(TM.getTargetTriple()),
        // TSInfo(),           // Target SelectionDAG
        InstrInfo(initializeSubtargetDependencies(CPU, FS)),
        FrameLowering(*this),
        TLInfo(TM, *this)   // Target Lowering
        {}

//===----------------------------------------------------------------------===//
//  
//===----------------------------------------------------------------------===//