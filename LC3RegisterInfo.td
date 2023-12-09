//===-- LC3RegisterInfo.td - LC3 Register defs ---------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the LC3 register file
//===----------------------------------------------------------------------===//


class LC3Reg<bits<16> Enc, string n> : Register<n> {
  let HWEncoding = Enc;
  let Namespace = "LC3";
}

// LC3 CPU Registers
class LC3GPRReg<bits<16> Enc, string n> : LC3Reg<Enc, n>;

//===----------------------------------------------------------------------===//
//@Registers
//===----------------------------------------------------------------------===//
// The register string, such as "9" or "gp" will show on "llvm-objdump -d"
//@ All registers definition
let Namespace = "LC3" in {
  //@ General Purpose Registers
  def ZERO : LC3GPRReg<0,  "zero">, DwarfRegNum<[0]>;
  def AT   : LC3GPRReg<1,  "1">,    DwarfRegNum<[1]>;
  def V0   : LC3GPRReg<2,  "2">,    DwarfRegNum<[2]>;
  def V1   : LC3GPRReg<3,  "3">,    DwarfRegNum<[3]>;
  def A0   : LC3GPRReg<4,  "4">,    DwarfRegNum<[4]>;
  def A1   : LC3GPRReg<5,  "5">,    DwarfRegNum<[5]>;
  def T9   : LC3GPRReg<6,  "t9">,   DwarfRegNum<[6]>;
  def T0   : LC3GPRReg<7,  "7">,    DwarfRegNum<[7]>;
  def T1   : LC3GPRReg<8,  "8">,    DwarfRegNum<[8]>;
  def S0   : LC3GPRReg<9,  "9">,    DwarfRegNum<[9]>;
  def S1   : LC3GPRReg<10, "10">,   DwarfRegNum<[10]>;
  def GP   : LC3GPRReg<11, "gp">,   DwarfRegNum<[11]>;
  def FP   : LC3GPRReg<12, "fp">,   DwarfRegNum<[12]>;
  def SP   : LC3GPRReg<13, "sp">,   DwarfRegNum<[13]>;
  def LR   : LC3GPRReg<14, "lr">,   DwarfRegNum<[14]>;
  def SW   : LC3GPRReg<15, "sw">,   DwarfRegNum<[15]>;
}