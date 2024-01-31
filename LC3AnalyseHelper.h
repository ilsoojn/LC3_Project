//===-- LC3AnalyseImmediate.h - Analyse Immediates ------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LC3_ANALYSE_HELPER_H
#define LC3_ANALYSE_HELPER_H

#include "LC3.h"
#include "llvm/ADT/SmallVector.h"
// #include "llvm/Support/DataTypes.h"

namespace llvm {

    class LC3AnalyseHelper {
    public:
        struct Inst {
            unsigned OperatorCode;
            unsigned ImmOperand;
            Inst(unsigned OperatorCode, unsigned ImmOperand);
        };
        typedef SmallVector<Inst, 5> InstSequence;
        
        bool isOpcodeImm5(unsigned opcode){ return (opcode == LC3::ADDri || opcode == LC3::ANDri); }
        bool isOpcodeImm6(unsigned opcode){ return (opcode == LC3::LDR || opcode == LC3::STR); }
        bool isOpcodeImm9(unsigned opcode){ return (opcode == LC3::BR || opcode == LC3::LEA 
                                                || opcode == LC3::LD || opcode == LC3::LDI
                                                || opcode == LC3::ST || opcode == LC3::STI); }
        bool isOpcodeImm11(unsigned opcode){ return (opcode == LC3::JSR); }

        /// Instruction Helper 
        ///   to handle a large immediate and/or to Load immediate to a register.
        /// This might be re-arranged and be a part of LC3InstrInfo in the future or not.
        const InstSequence &Analyse(unsigned OperatorCode, uint16_t ImmOperand);
    
    private:
        /// AddInstr - Instr
        void AddInstr(InstSequence &InstSeq, const Inst &I);

  };
}

#endif
