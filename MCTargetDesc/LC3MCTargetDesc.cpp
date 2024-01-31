//===-- LC3MCTargetDesc.cpp - LC3 Target Descriptions -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides LC3 specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "LC3MCTargetDesc.h"
// #include "LC3InstPrinter.h"
// #include "LC3MCAsmInfo.h"
// #include "LC3TargetStreamer.h"
#include "TargetInfo/LC3TargetInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "mctargetdesc"

#define GET_INSTRINFO_MC_DESC
#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "LC3GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "LC3GenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "LC3GenRegisterInfo.inc"

static MCInstrInfo *createLC3MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitLC3MCInstrInfo(X);
  return X;
}

// static MCAsmInfo *createLC3MCAsmInfo(const MCRegisterInfo &MRI,
//                                        const Triple &TT,
//                                        const MCTargetOptions &Options) {
//   MCAsmInfo *MAI = new LC3MCAsmInfo(TT);
//   unsigned Reg = MRI.getDwarfRegNum(LC3::SP, true);
//   MCCFIInstruction Inst = MCCFIInstruction::cfiDefCfa(nullptr, Reg, 0);
//   MAI->addInitialFrameState(Inst);
//   return MAI;
// }

static MCRegisterInfo *createLC3MCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitLC3MCRegisterInfo(X, LC3::SR);
  return X;
}

static MCSubtargetInfo *
createLC3MCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  if (CPU.empty())
    CPU = "LC3";
  return createLC3MCSubtargetInfoImpl(TT, CPU, /*TuneCPU*/ CPU, FS);
}

// static MCInstPrinter *createLC3MCInstPrinter(const Triple &T,
//                                               unsigned SyntaxVariant,
//                                               const MCAsmInfo &MAI,
//                                               const MCInstrInfo &MII,
//                                               const MCRegisterInfo &MRI) {
//   return new LC3InstPrinter(MAI, MII, MRI);
// }

// static MCTargetStreamer *createTargetAsmStreamer(MCStreamer &S,
//                                                  formatted_raw_ostream &OS,
//                                                  MCInstPrinter *InstPrint,
//                                                  bool isVerboseAsm) {
//   return new LC3TargetAsmStreamer(S, OS);
// }

// static MCTargetStreamer *createObjectTargetStreamer(MCStreamer &S, 
//                                                     const MCSubtargetInfo &STI) {
//   return new LC3TargetELFStreamer(S);
// }


extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeLC3TargetMC() {
  // Register the MC asm info.
  // RegisterMCAsmInfoFn X(getTheLC3Target(), createLC3MCAsmInfo);

  for (Target *T : {&getTheLC3Target()}) {
    // Register the MC instruction info.
    TargetRegistry::RegisterMCInstrInfo(*T, createLC3MCInstrInfo);

    // Register the MC register info.
    TargetRegistry::RegisterMCRegInfo(*T, createLC3MCRegisterInfo);

    // Register the MC subtarget info.
    TargetRegistry::RegisterMCSubtargetInfo(*T, createLC3MCSubtargetInfo);

    // Register the MCInstPrinter
    // TargetRegistry::RegisterMCInstPrinter(*T, createLC3MCInstPrinter);

    // // Register the MC Code Emitter.
    // TargetRegistry::RegisterMCCodeEmitter(*T, createLC3MCCodeEmitter);

    // // Register the asm backend.
    // TargetRegistry::RegisterMCAsmBackend(*T, createLC3AsmBackend);

    // // Register the asm streamer.
    // TargetRegistry::RegisterAsmTargetStreamer(*T, createTargetAsmStreamer);

    // // Register the object target streamer.
    // TargetRegistry::RegisterObjectTargetStreamer(*T, createObjectTargetStreamer);

  }// end for-target
}
