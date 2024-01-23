//===-- LC3InstrInfo.td - Target Description for LC3 Target -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes the LC3 instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

include "LC3InstrFormats.td"

//===----------------------------------------------------------------------===//
// LC3 DAG Nodes 
//===----------------------------------------------------------------------===//

// <(outs), (ins), "assembly string", [list pattern]>

// OPERANDS
// outs     :   list of definitions or outputs
// ins      :   list of uses or inputs
// operand  :   RegistrClass (GenRegs, ProcReg)
//          :   Immediate (i#imm)

// ASSEMBLY STRING
// Define Pattern, we see

// LIST PATTERN
// Matches Nodes in SelectionDAG
// Syntax   :   One pair of Parenthesis definions one note
//              Nodes has DAG operands with MVT type (i#, iPTR...)
//              Map DAG operands to MI operands

//===----------------------------------------------------------------------===//
// Type profiles
// SDTypeProfile < int NumResults, int NumOperands, list<SDTypeConstraint> Constraints >
//===----------------------------------------------------------------------===//
def SDT_LC3Jump         : SDTypeProfile<0, 1, [SDTCisVT<0, iPTR>]>;
def SDT_LC3CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i16>]>; // llvm built-in 
def SDT_LC3CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i16>, SDTCisVT<0, i16>]>; // llvm built-in 

//===----------------------------------------------------------------------===//
// Custom SDNodes (string ISD_opcode, SDTypeProfile, list<SDTypeProperty>, SDClass)
// Built-In: brcc, brcond, br, setcc, trap, ld, st ...
// https://github.com/llvm-mirror/llvm/blob/master/include/llvm/CodeGen/SDNodeProperties.td
//===----------------------------------------------------------------------===//
def LC3JumpSub      : SDNode<"LC3ISD::JmpSub", SDT_LC3Jump, [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;
def LC3Jump         : SDNode<"LC3ISD::Jump", SDT_LC3Jump, [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>; 
def LC3Not          : SDNode<"LC3ISD::NOT", SDTIntBinOp, []>;
def callseq_start   : SDNode<"ISD::CALLSEQ_START", SDT_LC3CallSeqStart, [SDNPHasChain, SDNPOutGlue]>;
def callseq_end     : SDNode<"ISD::CALLSEQ_END",   SDT_LC3CallSeqEnd, [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

// LLVM: MachineTypeValue.h // MCCodeEmiter

//===----------------------------------------------------------------------===//
//    Operand Definitions
//===----------------------------------------------------------------------===//
let OperandType = "OPERAND_IMMEDIATE" in
{
    // def immSExt5: PatLeaf<(imm), [{ return isInt<5>(N->getSExtValue()); }]>;
    def immZExt5 : Operand<i16>, ImmLeaf<i16, [{ return Imm >= 0 && Imm < 32; }]>;
    def LC3imm3 : Operand<i16>, ImmLeaf<i16, [{ return Imm >= 0 && Imm < 8; }]>;
}

let OperandType = "OPERAND_PCREL" in {
    def PCoffset11: Operand<i16>{ let EncoderMethod = "getJumpTargetOpValue"; }, ImmLeaf<i16, [{ return Imm >= 0 && Imm < 2048; }]>;
    def PCoffset9 : Operand<i16>, ImmLeaf<i16, [{ return Imm >= 0 && Imm < 512; }]>;
    def PCoffset6 : Operand<i16>, ImmLeaf<i16, [{ return Imm >= 0 && Imm < 64; }]>;
}

// Offset Calculating 
def basei   : ComplexPattern<iPTR, 2, "SelectAddrRI", [],[]>;   // <- BaseReg[x] + Immediate Offset
def pci     : ComplexPattern<iPTR, 1, "SelectAddrI", [],[]>;    // <- PC + Immediate Offset

// JSR
def JsrTarget   : Operand<OtherVT> {
  let EncoderMethod = "getJumpTargetOpValue";
  let OperandType = "OPERAND_PCREL";
}

// Branching Condition Code
def cc   : Operand<i16>{ 
    let PrintMethod = "printCondCodeOperand";
    let OperandNamespace = "LC3";
    let OperandType = "OPERAND_COND_CODE";
}

// Address Operands
def LC3MemBaseAsmOperand  : AsmOperandClass {
    let Name = "MemBaseOp";
    let ParserMethod = "parseMemOperand";
}
def LC3MemPcAsmOperand  : AsmOperandClass {
    let Name = "MemPcOp";
    let ParserMethod = "parseMemOperand";
}

def MemBaseOp     : Operand<iPTR>{
    let PrintMethod = "printMemOperand";        // from LC3InstPrinter.cpp
    let EncoderMethod = "getMemEncoding";       // from LC3MCCodeEmitter.cpp
    let ParserMatchClass = LC3MemBaseAsmOperand;    // from LC3AsmParser.cpp
    let MIOperandInfo = (ops GenRegs, i16imm);
}
def MemPcOp     : Operand<iPTR>{
    let PrintMethod = "printMemOperand";        // from LC3InstPrinter.cpp
    let EncoderMethod = "getMemEncoding";       // from LC3MCCodeEmitter.cpp
    let ParserMatchClass = LC3MemPcAsmOperand;    // from LC3AsmParser.cpp
    let MIOperandInfo = (ops i16imm);
}
  
//===----------------------------------------------------------------------===//
//  Pattern Fragments
//  PatFrag<(Input Node of any 'ops'), (Expended Node of specified 'Node'), Predicate>
//          eg (TargetSelectionDAG.td)
//          def not :   Patfrag<(ops node:$in), (xor node:$in, -1)>;
//                               not $in = xor $in, -1              ???
//===----------------------------------------------------------------------===//

class AlignedLoad<PatFrag Node>
    PatFrag<(ops node:$ptr), (Node node:$ptr), [{
    LoadSDNode *LD = cast<LoadSDNode>(N);
    return LD->getMemoryVT().getSizeInBits()/8 <= LD->getAlignment();
}]>;


//===----------------------------------------------------------------------===//
//  Arithmetic & Logic Unit: ADD & AND + NOT
//===----------------------------------------------------------------------===//
let Defs = [PSR] in {
    multiclass OpALU<bits<4> opcode, string opstr, SDNode opnode>{

        def rr :    LC3Inst<opcode,
                            (outs GenRegs:$dst), 
                            (ins GenRegs:$src1, GenRegs:$src2), 
                            !strconcat(opstr, "\t$dst, $src1, $src2"),
                            [
                                (set i16:$dst, (opnode i16:$src1, i16:$src2)),
                                (implicit PSR) // Modify Processor Status Register
                            ]>{
            bits<3> dst;
            bits<3> src1;
            bits<3> src2;
            let Inst{11-9} = dst;
            let Inst{8-6} = src1;
            let Inst{5} = 0;
            let Inst{4-3} = 0b00;
            let Inst{2-0} = src2;
        }
        
        def ri :    LC3Inst<opcode,
                            (outs GenRegs:$dst), 
                            (ins GenRegs:$src, immZExt5:$imm5), 
                            !strconcat(opstr, "\t$dst, $src, $imm5"),
                            [
                                (set i16:$dst, (opnode i16:$src, immZExt5:$imm5)),
                                (implicit PSR)
                            ]>{
            bits<3> dst;
            bits<3> src;
            bits<5> imm5;
            let Inst{11-9} = dst;
            let Inst{8-6} = src;
            let Inst{5} = 1;
            let Inst{4-0} = imm5;
        }
    } // multiclass OpALU
} // Defs = [PSR]

defm ADD : OpALU<0b0001, "ADD", add>;
defm AND : OpALU<0b0101, "AND", and>;

// NOT
let isBitcast = 1, Defs = [PSR] in {
    def NOT :   LC3BI<0b1001, 
                        (outs GenRegs:$reg),
                        (ins GenRegs:$base), 
                        "NOT \t$reg \t$base", 
                        [
                            (set i16:$reg, (not i16:$base)),
                            (implicit PSR)
                        ]>{
        let offset6 = 0b111111;
    }//NOT        
} // isBitcase = 1, Defs=[PSR]

//===----------------------------------------------------------------------===//
//  BRANCH : Unconditional IF NZP <- 111 or 000
//===----------------------------------------------------------------------===//
let isBranch = 1, isTerminator = 1, Uses = [PSR] in {
    def BR  : LC3CC<0b0000, (outs), (ins MemPcOp:$target, cc:$cc), 
                        "BR$cc $target", 
                        [(brcc bb:$target, LC3imm3:$cc)]>{
        bits<9> target;
        bits<3> cc;
        let Inst{11-9} = cc; // == n z p condition
        let Inst{8-0} = target;
    }
}
//===----------------------------------------------------------------------===//
//  RET : Return from SubRoutine
//  RTI : Return from Interrupt
//===----------------------------------------------------------------------===//

// variable_ops : it takes a varaible number of operands
// In this case, variable_ops <- 0 | 1
let isBranch = 1, isTerminator = 1, isReturn = 1, isBarrier = 1 in {

    def RET : LC3Return<0b1100, (outs), (ins), "RET", []>{
        let imm3 = 0b111;
    }// RET : PC <- R7

    def RTI : LC3Return<0b1000, (outs), (ins), "RTI", []>{
        let imm3 = 0b000;
    }
    // RTI : IF PSR[15] == 0, 
    //          THEN PC <- [R6], TMP <- [R6++], R6++, PSR <- TMP. 
    //          ELSE "Initiate a privilege mode excpetion"
    //       PC, PSR <- Two value is Popped out of the Stack
}
//===----------------------------------------------------------------------===//
//  JMP         :   Unconditional Jmp
//  JSR & JSRR  :   Jump to SubRoutine
//                  R7 = PC, Then GOTO... the label.
//===----------------------------------------------------------------------===//
let isTerminator = 1, isCall = 1, isBranch = 1 in {

    def JMP : LC3Inst<0b1100, (outs), (ins GenRegs:$baseR),
                    "JMP $baseR", []>{
        bits<3> baseR;
        let Inst{11-9} = 0b000;
        let Inst{8-6} = baseR;
        let Inst{5-0} = 0b000000;
    }// JMP R# : PC <- R#

    def JSR : LC3Inst<0b0100, (outs), (ins PCoffset11:$target),
                        "JSR $target", [(LC3JumpSub PCoffset11:$target)]>{
        bits<11> target;
        let Inst{11} = 1;
        let Inst{10-0} = target;
    }// JSR target : R7 = PC & PC = PC + SExt(target)

    def JSRR : LC3Inst<0b0100, (outs), (ins GenRegs:$baseR),
                        "JSR $baseR", [LC3JumpSub GenRegs:$baseR]>{
        bits<3> baseR;
        let Inst{11-9} = 0b000;
        let Inst{8-6} = baseR;
        let Inst{5-0} = 0b000000;
    }// JSRR Rn : R7 = PC & PC = Rn
}

//===----------------------------------------------------------------------===//
//  LOAD    :   LD, LDI, LDR
//===----------------------------------------------------------------------===//

class Load<bits<4>, RegisterClass RC, ValueType VT, string asmstr>
    : LC3PC9 <opcode, (outs RC:$reg), (ins MemPcOp:$PCoffset9), 
            !strconcat(asmstr, "\t$reg,\t$PCoffset9"), 
            [(set RC:$reg, (OpNode pci:$PCoffset9) )]>{
    let DecoderMethod = "DecodeMemory";
}

let Defs = [PSR] in {
    // reg <- [PC + PCoffset9]
    def LD    :   Load<0b0010, GenRegs, i16, "LD", >{
    }

    // reg <- [[PC + PCoffset9]]
    def LDI     :   Load<0b1010, GenRegs, i16, "LDI", >{>{
    }

    // reg <- [base + offset6]
    def  LDR    :   Load<0b0110, (outs GenRegs:$reg), (ins GenRegs:$base, MemBaseOp:$offset6),
                            "LDR\t$reg, $base, $offset6",
                            [(set i16:$reg, (load i16:$base, MemBaseOp:$offset6) )]>{
    } 
} // Defs = [PSR], based on $reg value (NZP)

//===----------------------------------------------------------------------===//
//  STORE   :   ST, STI, STR
//===----------------------------------------------------------------------===//
// [PC + PCoffset9] <- reg
def STORE   :   OpMem<0b0011, (outs GenRegs:$reg), (ins MemPcOp:$PCoffset9),
                        "ST $reg, $PCoffset9",
                        [(store i16:$reg, pci:$PCoffset9)]>{
}

// [[PC + PCoffset9]] <- reg
def STI     :   OpMem<0b1011, (outs GenRegs:$reg), (ins MemPcOp:$PCoffset9),
                        "STI $reg, $PCoffset9",
                        [(store i16:$reg, pci:$PCoffset9)]>{
}

// [base + offset6] <- reg
def STR     :   LC3BI<0b0111, (outs GenRegs:$reg), (ins GenRegs:$base, MemBaseOp:$offset6),
                        "ST $reg, $offset6",
                        [(store i16:$reg, i16:$base, MemBaseOp:$offset6)]>{
}

//===----------------------------------------------------------------------===//
//  LEA (Load Effective Address)
//===----------------------------------------------------------------------===//
// reg <- PC + PCoffset9
let Defs = [PSR] in {
    def LEA    :   LC3PC9<0b1110, (outs GenRegs:$reg), (ins MemPcOp:$PCoffset9),
                            "LEA\t$reg, $PCoffset9",
                            [(set GenRegs:$reg,  pci:$PCoffset9)]>{
    }
} // Defs = [PSR] for LEA