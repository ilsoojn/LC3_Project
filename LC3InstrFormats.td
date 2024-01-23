//===-- LC3InstrFormats.td - LC3 Instruction Formats ----- =- tablegen -=-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Describe LC3 Instructions format
//  opcode      : operation code
//  dst         : destination register
//  src         : source register
//  baseR       : base register which will be used instead of PC
//  imm#        : #-bit immediate 
//  offset#     : #-bit offset, typically used with baseR
//  PCoffset#   : #-bit offset, counted from PC 
//  SR          : Program's Status Register
//===----------------------------------------------------------------------===//

// LC3 Format
class LC3Inst<bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : Instruction {

    field bits<16> Inst; // 16-bit instruction ISA
    let Namespace = "LC3";

    let Size = 4;               // Size of encoded instruction, or zero if the size cannot be determined from the opcode.
    let Inst{15-12} = opcode;

    let OutOperandList = outs;  // def
    let InOperandList = ins;    // use
    let AsmString = asmstr;
    let Pattern = pattern;      // "Set to the DAG attern for this instruction, if we know of one, otherwise, uninitialized."

}

// LC3PC9 Format < opcode | dst/src | PCoffset9 >
// DST/SRC <- (op)[PC + PCoffset9]
class LC3PC9 <bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : LC3Inst <opcode, outs, ins, asmstr, pattern>{
    
    bits<3> reg;
    bits<9> PCoffset9;

    let Inst{15-12} = opcode;
    let Inst{11-9} = reg;
    let Inst{8-0} = PCoffset9;

}

// LC3BI Format < opcode | dst/src | base_reg | offset6 >
// DST/SRC <- (op) [base + PCoffset6]
class LC3BI <bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : LC3Inst <opcode, outs, ins, asmstr, pattern>{

    bits<3> reg;
    bits<3> base;
    bits<6> offset6;

    let Inst{15-12} = opcode;
    let Inst{11-9}  = reg;
    let Inst{8-6}   = base;
    let Inst{5-0}   = offset6;
}

// LC3RET Format < opcode | 000 | 000/111 | 000000 >
class LC3Return <bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : LC3Inst <opcode, outs, ins, asmstr, pattern>{
    
    bits<3> imm3;

    let Inst{15-12} = opcode;
    let Inst{11-9}  = 0b000;
    let Inst{8-6}   = imm3;     // RET 111, RTI 000
    let Inst{5-0}   = 0b000000;
}

// LC3CC Format < opcode | nzp | pc_offset9 >
// class LC3CC <bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
//     : LC3Inst <opcode, outs, ins, asmstr, pattern>{
    
//     bits<1> n;
//     bits<1> z;
//     bits<1> p;
//     bits<9> PCoffset9;

//     let Inst{15-12} = opcode;
//     let Inst{11}    = n;
//     let Inst{9}     = z;
//     let Inst{10}    = p;
//     let Inst{8-0}   = PCoffset9;

// //    let Inst{11-9}  = cond; // with bits<3> cond;
// }

// LC3PC11 Format < opcode | 1 | pc_offset11 >
class LC3PC11 <bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : LC3Inst <opcode, outs, ins, asmstr, pattern>{
    
    bits<11> PCoffset11;

    let Inst{15-12} = opcode;
    let Inst{11}    = 0b1;
    let Inst{10-0}  = PCoffset11;
}

// LC3TRAP Format < opcode | 0000 | trabvect8 >
class LC3TRAP <bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : LC3Inst <opcode, outs, ins, asmstr, pattern>{
    
    bits<8> trapvect;

    let Inst{15-12} = opcode;
    let Inst{11-8}  = 0b0000;
    let Inst{7-0}   = trapvect;

}

class LC3PseudoInst<bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern> 
    : LC3Inst<opcode, outs, ins, asmstr, pattern> {
    let isPseudo = 1;
    let isCodeGenOnly = 1;
}

// class Inst<> : LC3Inst<opcode, outs, ins, opcodestr, asmstr, pattern> {}
