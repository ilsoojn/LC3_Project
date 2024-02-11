//===-- LC3.td - Describe the LC3 Target Machine -------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// LC3 Subtarget features.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Register, Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//

include "LC3RegisterInfo.td"
include "LC3InstrInfo.td"
include "LC3CallingConv.td"
//include "LC3Schedule.td"

def LC3InstrInfo : InstrInfo;

def LC3AsmParser : AsmParser {
  bit ShouldEmitMatchRegisterName = 0;
}

def LC3AsmParserVariant : AsmParserVariant {
  let RegisterPrefix = "%"; // Recognize hard coded registers
}

//===----------------------------------------------------------------------===//
// LC3 processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features>
 : Processor<Name, NoItineraries, Features>;

def : Proc<"generic",         []>;

//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//

// def LC3AsmWriter : AsmWriter {
//   string AsmWriterClassName  = "InstPrinter";
//   int PassSubtarget = 1;
//   int Variant = 0;
// }

def LC3 : Target {
  // Pull in Instruction Info:
  let InstructionSet = LC3InstrInfo;
  let AssemblyParsers  = [LC3AsmParser];
  let AssemblyParserVariants = [LC3AsmParserVariant];
  // let AssemblyWriters = [LC3AsmWriter];
}
