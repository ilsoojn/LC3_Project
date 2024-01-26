//===-- LC3MCTargetDesc.h - LC3 Target Descriptions ---------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_LC3_MCTARGETDESC_LC3MCTARGETDESC_H
#define LLVM_LIB_TARGET_LC3_MCTARGETDESC_LC3MCTARGETDESC_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/DataTypes.h"

#include <memory>

namespace llvm {
// class MCAsmBackend;
// class MCCodeEmitter;
// class MCContext;
class MCInstrInfo;
// class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
// class MCTargetOptions;
class Target;

// MCCodeEmitter *createLC3MCCodeEmitter(const MCInstrInfo &MCII,
//                                         MCContext &Ctx);
// MCAsmBackend *createLC3AsmBackend(const Target &T, const MCSubtargetInfo &STI,
//                                     const MCRegisterInfo &MRI,
//                                     const MCTargetOptions &Options);
// std::unique_ptr<MCObjectTargetWriter> createLC3ELFObjectWriter(bool Is64Bit,
//                                                                  uint8_t OSABI);
} // End llvm namespace

// Defines symbolic names for LC3 registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "LC3GenRegisterInfo.inc"

// Defines symbolic names for the LC3 instructions.
#define GET_INSTRINFO_ENUM
#include "LC3GenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "LC3GenSubtargetInfo.inc"

#endif
