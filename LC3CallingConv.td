//===-- LC3CallingConv.td - Calling Conventions for LC3 --*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for LC3 architecture.
//===----------------------------------------------------------------------===//

// error: Variable not defined: 'R0'
/// CCIfSubtarget - Match if the current subtarget has a feature F.
class CCIfSubtarget<string F, CCAction A>:
  CCIf<!strconcat("State.getTarget().getSubtarget<LC3Subtarget>().", F), A>;

def CC_LC3 : CallingConv<[
  CCIfType<[i16], CCAssignToReg<[R0, R1, R2, R3, R4]>>,
  CCAssignToStack<5, 2> // <int size, int align_byte>
]>;

/// CCDelegateTo - This action invokes the specified sub-calling-convention.  
///                It is successful if the specified CC matches.
def RetCC_LC3 : CallingConv<[
  CCDelegateTo<CC_LC3>
]>;

def CSR : CalleeSavedRegs<(add LR, FP, (sequence "R%u", 0, 4))>;