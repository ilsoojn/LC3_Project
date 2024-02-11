//===-- LC3InstrInfo.h - LC3 Instruction Information --------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the LC3 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_LC3_LC3INSTRINFO_H
#define LLVM_LIB_TARGET_LC3_LC3INSTRINFO_H

#include "LC3RegisterInfo.h"
#include "LC3.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include <cstdint>

#define GET_INSTRINFO_HEADER
#include "LC3GenInstrInfo.inc"

namespace llvm {

class LC3Subtarget;

#define NZP flase
namespace LC3CC {

    #if NZP
    enum CondCodes {
        BN = 4,    // 0b_100 a < 0
        BNZ = 6,   // 0b_110 a <= 0
        BZ = 2,    // 0b_010 a = 0
        BZP = 3,   // 0b_011 a >= 0
        BP = 1,    // 0b_001 a > 0
        BNP = 5,   // 0b_101 a != 0
        BNZP = 7,  // 0b_111 Unconditional Branching
        BINVALID
    };
    #else
    enum CondCodes {
        BL = 4,    // 0b_100 a < 0
        BLE = 6,   // 0b_110 a <= 0
        BEQ= 2,    // 0b_010 a = 0
        BGE = 3,   // 0b_011 a >= 0
        BG = 1,    // 0b_001 a > 0
        BNE = 5,   // 0b_101 a != 0
        BUNCOND = 7,  // 0b_111 Unconditional Branching
        BINVALID
    };
    #endif

}

class LC3InstrInfo : public LC3GenInstrInfo {

    virtual void anchor();
    const LC3RegisterInfo RI;
    const LC3Subtarget& Subtarget;

public:
    explicit LC3InstrInfo(LC3Subtarget &STI);

    /// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
    /// such, whenever a client has an instance of instruction info, it should
    /// always be able to get register info as well (through this method).
    ///
    const LC3RegisterInfo &getRegisterInfo() const { return RI; }

    /// isLoadFromStackSlot - If the specified machine instruction is a direct
    /// load from a stack slot, return the virtual or physical register number of
    /// the destination along with the FrameIndex of the loaded stack slot.  If
    /// not, return 0.  This predicate must return 0 if the instruction has
    /// any side effects other than loading from the stack slot.
    unsigned isLoadFromStackSlot(const MachineInstr &MI,
                                int &FrameIndex) const override;

    /// isStoreToStackSlot - If the specified machine instruction is a direct
    /// store to a stack slot, return the virtual or physical register number of
    /// the source reg along with the FrameIndex of the loaded stack slot.  If
    /// not, return 0.  This predicate must return 0 if the instruction has
    /// any side effects other than storing to the stack slot.
    unsigned isStoreToStackSlot(const MachineInstr &MI,
                                int &FrameIndex) const override;

    void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                    const DebugLoc &DL, MCRegister DestReg, MCRegister SrcReg,
                    bool KillSrc) const override;

    void storeRegToStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MBBI,
                            Register SrcReg, bool isKill, int FrameIndex,
                            const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI, Register VReg) const override {
        storeRegToStack(MBB, MBBI, SrcReg, isKill, FrameIndex, RC, TRI, 0);
    }

    void storeRegToStack(MachineBasicBlock &MBB,
                        MachineBasicBlock::iterator MBBI, 
                        Register SrcReg, bool isKill, int FrameIndex,
                        const TargetRegisterClass *RC,
                        const TargetRegisterInfo *TRI,
                        int16_t Offset) const;

    void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MBBI,
                            Register DestReg, int FrameIndex,
                            const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI, Register VReg) const override {
        loadRegFromStack(MBB, MBBI, DestReg, FrameIndex, RC, TRI, 0);
    }

    void loadRegFromStack(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MBBI, 
                            Register DestReg, int FrameIndex, 
                            const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI,                            int16_t Offset) const;


    /// GetInstSize - Return the number of bytes of code the specified
    /// instruction may be.  This returns the maximum number of bytes.
    unsigned getInstSizeInBytes(const MachineInstr &MI) const override;

    // absent of MOV Reg #Imm, we use ADD instead
    Register loadImmediate(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                            const DebugLoc &DL, int16_t Imm) const;

    //===----------------------------------------------------------------------===//
    //                            BRANCH INSTRUCTION                              //
    //===----------------------------------------------------------------------===//
    const MCInstrDesc &getBrCond(LC3CC::CondCodes CC) const;
    
    LC3CC::CondCodes getBrCond(MachineOperand MachineOperand) const;

    LC3CC::CondCodes getOppositeBranchCondition(LC3CC::CondCodes CC) const;
    
    bool reverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const override;

    /// Determine if the branch target is in range (offset for JSR and BR)
    bool isBranchOffsetInRange(unsigned BRCC, int64_t Offset) const override;

    MachineBasicBlock *getBranchDestBlock(const MachineInstr &MI) const override;

    bool analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
                        MachineBasicBlock *&FBB,
                        SmallVectorImpl<MachineOperand> &Cond,
                        bool AllowModify = false) const override;

    unsigned removeBranch(MachineBasicBlock &MBB,
                        int *BytesRemoved = nullptr) const override;

    unsigned insertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                        MachineBasicBlock *FBB, ArrayRef<MachineOperand> Cond,
                        const DebugLoc &DL,
                        int *BytesAdded = nullptr) const override;

}; // end LC3InstrInfo class

} // end namespace llvm

#endif
