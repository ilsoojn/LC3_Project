//===-- LC3AnalyzeImmediate.cpp - Analyze Immediates ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "LC3AnalyseHelper.h"
#include "llvm/Support/MathExtras.h"
#include <stdint.h>

using namespace llvm;

#define DEBUG_TYPE "analysehelper"

LC3AnalyseHelper::Inst::Inst(unsigned OPC, unsigned OPND) : OperatorCode(OPC), ImmOperand(OPND) {}

// Add I to the instruction sequences.
void LC3AnalyseHelper::AddInstr(InstSequence &InstSeq, const Inst &I) {
    InstSeq.push_back(I);
}

const LC3AnalyseHelper::InstSequence
&LC3AnalyseHelper::Analyse(unsigned opcode, uint16_t opndi) {

    InstSequence InstSeq;

    switch(opcode){
        // Immediate 5
        case LC3::ADDri:
        case LC3::ANDri:
            if(isIntN(5, opndi))
            {
                InstSeq.push_back(Inst(opcode, opndi));
            }
            else
            {
                opcode = (opcode == LC3::ADDri ? LC3::ADDrr : LC3::ANDrr);
                
                InstSeq.push_back(Inst(opcode, opndi));
            }
            break;

        // Immediate 6
        case LC3::LDR:
        case LC3::STR:
            if(isIntN(6, opndi))
            break;
        
        // Immediate 9
        case LC3::BR:
        case LC3::LEA:
        case LC3::LD:
        case LC3::LDI:
        case LC3::ST:
        case LC3::STI:
            if(isIntN(9, opndi))
            break;

        // Immediate 11
        case LC3::JSR:
            if(isIntN(11, opndi))
            break;
        
    }// switch(opcode)



    return InstSeq;
}
