//===-- LC3RegisterInfo.td - LC3 Register defs --------*- tablegen -*-==//
 //
 // Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 // See https://llvm.org/LICENSE.txt for license information.
 // SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 //
 //===----------------------------------------------------------------------===//
 //
 // Declarations that describe the LC3 register files
 //
 //===----------------------------------------------------------------------===//


class LC3Reg<bits<16> Enc, string n> : Register<n> {
    let HWEncoding = Enc;   // for tablegen( -gen-emitter) in CMakeLists.txt
    let Namespace = "LC3";
}

// Integer R0-R7 Registers
class LC3GPReg<bits<16> Enc, string n> : LC3Reg<Enc, n>;

// Processor Registers
class LC3PReg<bits<16> Enc, string n> : LC3Reg<Enc, n>;

//===----------------------------------------------------------------------===//
// Registers
//===----------------------------------------------------------------------===//
// The register string, such as "R0" or "PC" will show on "llvm-objdump -d"

let Namespace = "LC3" in {

    // -------------------------------------
    // DwarfRegNum<[]> assign value "that are used by gcc, gdb, or a debug information writer to identify a register"
    // -1 :: gcc Undefined
    // -2 :: register number is invalid for the mode.
    // Example from X86RegisterInfo.h
    // eg) def EAX: X86Reg <"eax", 0, [AX]>, DwarfRegNum<[-2, 0, 0]>;
    //     For X86-64, X86-32/Darwin, Generic X86-32/Linux & debug info on X86-32/Darwin

    // General Purpose Register Rn
    def R0  :   LC3GPReg<0, "R0">;
    def R1  :   LC3GPReg<1, "R1">;
    def R2  :   LC3GPReg<2, "R2">;
    def R3  :   LC3GPReg<3, "R3">;
    def R4  :   LC3GPReg<4, "R4">;
    def R5  :   LC3GPReg<5, "R5">;    // FP : Frame Pointer
    def R6  :   LC3GPReg<6, "R6">;    // SP : Stack Pointer
    def R7  :   LC3GPReg<7, "R7">;    // Return Addr / Goto Addr for RET/JSR

    // Special Registers (Status / Processor)
    // def MAR  :   Register<, "MAR">;
    // def MDR  :   Register<, "MDR">;
    
    def PC  :   LC3PReg<0, "PC">;
    // Processor Status Register. [15: privilege mode, 2: conditional N, 1: conditional Z, 0: conditional P]
    def SR :   LC3PReg<1, "SR">;  
}

//===----------------------------------------------------------------------===//
// Registers Class
//===----------------------------------------------------------------------===//

// General Purpose Register
def GenRegs : RegisterClass<"LC3", [i16], 16,
    (add (sequence "R%u", 0, 7))>;

// Processor's Register
def ProcRegs : RegisterClass<"LC3", [i16], 16, (add SR, PC)>;

