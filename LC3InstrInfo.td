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
// operand  :   RegistrClass (GPR, ProcReg)
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
def SDT_LC3BranchCC     : SDTypeProfile<0, 2, [SDTCisVT<0, OtherVT>, SDTCisVT<1, i16>]>;
def SDT_LC3Jump         : SDTypeProfile<0, 1, [SDTCisVT<0, iPTR>]>;
def SDT_LC3CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i16>]>; // llvm built-in 
def SDT_LC3CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i16>, SDTCisVT<0, i16>]>; // llvm built-in 

//===----------------------------------------------------------------------===//
// Custom SDNodes (string ISD_opcode, SDTypeProfile, list<SDTypeProperty>, SDClass)
// Built-In: brcc, brcond, br, setcc, trap, ld, st ...
// https://github.com/llvm-mirror/llvm/blob/master/include/llvm/CodeGen/SDNodeProperties.td
//===----------------------------------------------------------------------===//
def LC3BRnzp        : SDNode<"LC3ISD::BRNZP", SDT_LC3BranchCC, [SDNPHasChain, SDNPInGlue]>;
def LC3JumpSub      : SDNode<"LC3ISD::JMPSUB", SDT_LC3Jump, [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;
def LC3Jump         : SDNode<"LC3ISD::JUMP", SDT_LC3Jump, [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>; 
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
    def PCoffset11: Operand<i16>, ImmLeaf<i16, [{ return Imm >= 0 && Imm < 2048; }]>;
    def PCoffset9 : Operand<i16>, ImmLeaf<i16, [{ return Imm >= 0 && Imm < 512; }]>;
    def PCoffset6 : Operand<i16>, ImmLeaf<i16, [{ return Imm >= 0 && Imm < 64; }]>;
    def TrapVect8 : Operand<i16>, ImmLeaf<i16, [{ return Imm >= 0 && Imm < 256; }]>;
    def JsrTarget   : Operand<OtherVT> { let EncoderMethod = "getJumpTargetOpValue"; }
}

// Branching Condition Code
def NZP   : Operand<i16>{ 
    let PrintMethod = "printCondCodeOperand";
    // let OperandNamespace = "LC3";
}
def BRTarget : Operand<OtherVT> {
  let EncoderMethod = "getBranchTargetOpValue";
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

let OperandType = "OPERAND_MEMORY" in {
    class LC3MemOperand<RegisterClass RC> : RegisterOperand<RC>;
    def MemReg  : LC3MemOperand<GPR>;
    def MEM     : Operand<iPTR>{
        let PrintMethod = "printMemOperand";        // from LC3InstPrinter.cpp
        let MIOperandInfo = (ops GPR, PCoffset6);
        let EncoderMethod = "getMemOpValue";
    }
    def MemBaseOp   : Operand<iPTR>{
        let PrintMethod = "printMemOperand";        // from LC3InstPrinter.cpp
        let ParserMatchClass = LC3MemBaseAsmOperand;    // from LC3AsmParser.cpp
        let MIOperandInfo = (ops ptr_rc, PCoffset6); // ptr_rc from Target_td (register for pointer?)
    }
    def MemPcOp     : Operand<iPTR>{
        let PrintMethod = "printMemOperand";        // from LC3InstPrinter.cpp
        let ParserMatchClass = LC3MemPcAsmOperand;    // from LC3AsmParser.cpp
        let MIOperandInfo = (ops PCoffset9);
    }
}

//===----------------------------------------------------------------------===//
//  Complex Pattern
//  PatFrag< 
//      Ty         : (ValueType)            ReturnValueType, 
//      NumOperands: (int)                  #_of_operands_returned_by_the_function
//      SelectFunc : (string)               name of Funciton used to mattern match max
//      RootNodes  : (list<SDNode>)         list of sub-dag node to match (default = [])
//      Properties : (list<SDNodeProperty>) (default = [])>
//  eg (TargetSelectionDAG.td)
//  def not :   Patfrag<(ops node:$in), (xor node:$in, -1)>;
//                               not $in = xor $in, -1              ???
//===----------------------------------------------------------------------===//
def addrBase    : ComplexPattern<iPTR, 2, "SelectAddrRegImm", [],[]>;   // <- BaseReg[x] + Immediate Offset
def addrPC      : ComplexPattern<iPTR, 1, "SelectAddr", [frameindex],[]>;    // <- PC + Immediate Offset
def ADDRESS     : ComplexPattern<iPTR, 2, "SelectAddrRegReg", [frameindex],[SDNPWantParent]>; // frameIndex match for Load/Store from Stack

//===----------------------------------------------------------------------===//
//  Pseudo
//===----------------------------------------------------------------------===//

// What are they? "adjust Stack or SP(R6)" may rely on?
// Set before and after the “for loop”
// ValueType i16imm & timm is from the TargetSelectionDAG.td
let Defs = [SP], Uses = [SP] in {
def ADJCALLSTACKDOWN    : LC3PseudoInst<(outs), (ins i16imm:$amt1, i16imm:$amt2),
                                  "!ADJCALLSTACKDOWN $amt1",
                                  [(callseq_start timm:$amt1, timm:$amt2)]>;
def ADJCALLSTACKUP      : LC3PseudoInst<(outs), (ins i16imm:$amt1, i16imm:$amt2),
                                  "!ADJCALLSTACKUP $amt1 $amt2",
                                  [(callseq_end timm:$amt1, timm:$amt2)]>;
}
//===----------------------------------------------------------------------===//
//  Pattern Fragments
//  PatFrag<(Input Node of any 'ops'), (Expended Node of specified 'Node'), Predicate>
//          eg (TargetSelectionDAG.td)
//          def not :   Patfrag<(ops node:$in), (xor node:$in, -1)>;
//                               not $in = xor $in, -1              ???
//===----------------------------------------------------------------------===//
/*
class AlignedLoad<PatFrag Node>
    PatFrag<(ops node:$ptr), (Node node:$ptr), [{
    LoadSDNode *LD = cast<LoadSDNode>(N);
    return LD->getMemoryVT().getSizeInBits()/8 <= LD->getAlignment();
}]>;

class AlignedStore<PatFrag Node>
    PatFrag<(ops node:$ptr node:$addr), (Node node:$ptr node:$addr), [{
    LoadSDNode *LD = cast<LoadSDNode>(N);
    return LD->getMemoryVT().getSizeInBits()/8 <= LD->getAlignment();
}]>;
*/
//===----------------------------------------------------------------------===//
//  Arithmetic & Logic Unit: ADD & AND + NOT
//===----------------------------------------------------------------------===//
let Defs = [SR] in {
    // (implicit SR) at the Pattern is to indicate it Modify Processor Status Register

    multiclass OpALU<bits<4> opcode, string opstr, SDNode opnode>{

        def rr  : LC3Base6<opcode, (outs GPR:$reg), (ins GPR:$base, GPR:$src2), 
                        !strconcat(opstr, "\t$dst, $base, $src2"),
                        [(set i16:$reg, (opnode i16:$base, i16:$src2)), (implicit SR)]>{
            
            bits<3> src2;
            let Inst{5} = 0;
            let Inst{4-3} = 0b00;
            let Inst{2-0} = src2;
        }
        
        def ri  : LC3Base6<opcode, (outs GPR:$reg), (ins GPR:$base, immZExt5:$imm5), 
                        !strconcat(opstr, "\t$reg, $base, $imm5"),
                        [(set i16:$reg, (opnode i16:$base, immZExt5:$imm5)), (implicit SR)]>{
            
            bits<5> imm5;
            let Inst{5} = 1;
            let Inst{4-0} = imm5;
        }
    } // multiclass OpALU
} // Defs = [SR]

// IR defined in include/llvm/Target/TargetSelectionDAG.td
defm ADD : OpALU<0b0001, "ADD", add>;
defm AND : OpALU<0b0101, "AND", and>;

// NOT
    def NOT : LC3Base6<0b1001, (outs GPR:$reg), (ins GPR:$base), 
                        "NOT \t$reg \t$base", 
                        [(set i16:$reg, (not i16:$base)), (implicit SR)]>
                        {
                            let Inst{5-0} = 0b111111;
                            let isBitcast = 1;
                            let Defs = [SR];
                        }//NOT        

//===----------------------------------------------------------------------===//
//  BRANCH : Unconditional IF NZP <- 111 or 000
//===----------------------------------------------------------------------===//
    def BR  : LC3Inst<(outs), (ins BRTarget:$target, NZP:$condcode), 
                        "BR $cc $target", 
                        [(LC3BRnzp bb:$target, imm:$condcode)], FormOther>{

                        let Opcode = 0b0000;
                        
                        bits<9> target;
                        bits<3> condcode;
                        let Inst{11-9} = condcode; // == n z p condition
                        let Inst{8-0} = target;

                        let isTerminator = 1;
                        let Uses = [SR];
                    }
//===----------------------------------------------------------------------===//
//  RET : Return from SubRoutine
//  RTI : Return from Interrupt
//===----------------------------------------------------------------------===//

// variable_ops : it takes a varaible number of operands
// In this case, variable_ops <- 0 | 1
let isBranch = 1, isTerminator = 1, isReturn = 1, isBarrier = 1 in {

    def RET : LC3Base6<0b1100, (outs), (ins), "RET", []>{
        let reg = 0b000;
        let base = 0b111;
        let Inst{5-0} = 0b000000;
    }
    def RTI : LC3Base6<0b1000, (outs), (ins), "RTI", []>{
        let reg = 0b000;
        let base = 0b000;
        let Inst{5-0} = 0b000000;
    }
    // def RET : LC3Return<0b1100, (outs), (ins), "RET", []>{
    //     let imm3 = 0b111;
    // }// RET : PC <- LR(R7)

    // def RTI : LC3Return<0b1000, (outs), (ins), "RTI", []>{
    //     let imm3 = 0b000;
    // }
    // RTI : IF SR[15] == 0, 
    //          THEN PC <- [SP(R6)], TMP <- [SP(R6)++], SP(R6)++, SR <- TMP. 
    //          ELSE "Initiate a privilege mode excpetion"
    //       PC, SR <- Two value is Popped out of the Stack
}
//===----------------------------------------------------------------------===//
//  JMP         :   Unconditional Jmp
//  JSR & JSRR  :   Jump to SubRoutine
//                  LR(R7) = PC, Then GOTO... the label.
//===----------------------------------------------------------------------===//
let isTerminator = 1, isCall = 1, isBranch = 1 in {

    def JMP : LC3Base6<0b1100, (outs), (ins GPR:$base),
                        "JMP $base", 
                        []>{
                            let reg = 0b000;
                            let Inst{5-0} = 0b000000;
                        }// JMP R# : PC <- R#

    def JSR : LC3PC11<0b0100, (outs), (ins PCoffset11:$target),
                        "JSR $target", 
                        [(LC3JumpSub PCoffset11:$target)]>{
                            bits<11> target;
                            let PCoffset11 = target;
                        }// JSR target : LR(R7) = PC & PC = PC + SExt(target)

    def JSRR : LC3Base6<0b0100, (outs), (ins GPR:$base),
                        "JSR $base", 
                        [(LC3JumpSub GPR:$base)]>{
                            let reg = 0b000;
                            let Inst{5-0} = 0b000000;
                        }// JSRR Rn : LR(R7) = PC & PC = Rn
}

//===----------------------------------------------------------------------===//
//  LOAD    :   LD, LDI, LDR
//===----------------------------------------------------------------------===//

// Load class is from Sparc Load F3_2, where we print "LD[I] $reg $addr"
// class Load<bits<4>, RegisterClass RC, ValueType VT, string opstr> 
class Load<bits<4> opcode, string opstr>
    : LC3PC9 <opcode, (outs GPR:$reg), (ins MemPcOp:$PCoffset9), 
            !strconcat(opstr, "\t$reg,\t$PCoffset9"), 
            [(set i16:$reg, (load addrPC:$PCoffset9))]>;

let Defs = [SR] in {
    
    def LD  : Load<0b0010, "LD">;   // reg <- [PC + PCoffset9]
    def LDI : Load<0b1010, "LDI">;  // reg <- [[PC + PCoffset9]]

    // LDR is from RISCV Load_ri, where we print "LDR $reg $base $offset6"
    // reg <- [base + offset6]
    def LDR : LC3Base6<0b0110, (outs GPR:$reg), (ins MemReg:$base, PCoffset6:$offset6),
                            "LDR\t$reg,\t$base, $offset6",
                            [(set i16:$reg, (load (ptradd i16:$base, addrBase:$offset6)))]>
                            {
                                bits<6> offset6;
                                let Inst{5-0} = offset6;
                            }
} // Defs = [SR], based on $reg value (NZP)

//===----------------------------------------------------------------------===//
//  STORE   :   ST, STI, STR
//===----------------------------------------------------------------------===//

// Store class is from Sparc Store F3_2, where we print "ST[I] $reg $addr"
// class Store<bits<4>, RegisterClass RC, ValueType VT, string opstr> 
class Store<bits<4> opcode, string opstr>
    : LC3PC9 <opcode, (outs), (ins GPR:$reg, MemPcOp:$PCoffset9), 
            !strconcat(opstr, "\t$reg,\t$PCoffset9"), 
            [(store i16:$reg, addrPC:$PCoffset9)]>;

def ST  : Store<0b0011, "ST">;  // [PC + PCoffset9] <- reg
def STI : Store<0b1011, "STI">; // [[PC + PCoffset9]] <- reg

// STR is from RISCV Store_ri, where we print "STR $reg $base $offset6"
// [base + offset6] <- reg
def STR : LC3Base6<0b0111, (outs), (ins GPR:$reg, MemReg:$base, PCoffset6:$offset6),
                        "STR\t$reg,\t$base, $offset6",
                        [(store i16:$reg, (ptradd i16:$base, PCoffset6:$offset6))]>
                        {
                            bits<6> offset6;
                            let Inst{5-0} = offset6;
                        }

//===----------------------------------------------------------------------===//
//  LEA (Load Effective Address)
//===----------------------------------------------------------------------===//
// reg <- PC + PCoffset9
let Defs = [SR] in {
    def LEA : LC3PC9<0b1110, (outs GPR:$reg), (ins MemPcOp:$PCoffset9),
                    "LEA\t$reg, $PCoffset9",
                    [(set GPR:$reg,  addrPC:$PCoffset9)]>;
} // Defs = [SR] for LEA

//===----------------------------------------------------------------------===//
//  TRAP
//===----------------------------------------------------------------------===//

