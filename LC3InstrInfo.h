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
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "LC3GenInstrInfo.inc"

namespace llvm {

class LC3Subtarget;

namespace CondCode {

// NZP
enum CondCode {
    COND_n,    // a < 0
    COND_nz,   // a <= 0
    COND_z,    // a = 0
    COND_zp,   // a >= 0
    COND_p,    // a > 0
    COND_np,   // a != 0
    COND_nzp,  // Unconditional Branching
    COND_,     // Unconditional Branching
    COND_INVALID
};

CondCode getOppositeBrCond(CondCode);
unsigned getBrCond(CondCode CC);

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
/*
    /// isLoadFromStackSlot - If the specified machine instruction is a direct
    /// load from a stack slot, return the virtual or physical register number of
    /// the destination along with the FrameIndex of the loaded stack slot.  If
    /// not, return 0.  This predicate must return 0 if the instruction has
    /// any side effects other than loading from the stack slot.
    unsigned isLoadFromStackSlot(const MachineInstr &MI,
                                int &FrameIndex) const {
        switch(MI.getOpcode()){
            default:
                return 0;
        }
    }

    /// isStoreToStackSlot - If the specified machine instruction is a direct
    /// store to a stack slot, return the virtual or physical register number of
    /// the source reg along with the FrameIndex of the loaded stack slot.  If
    /// not, return 0.  This predicate must return 0 if the instruction has
    /// any side effects other than storing to the stack slot.
    unsigned isStoreToStackSlot(const MachineInstr &MI,
                                int &FrameIndex) const override;

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

    bool
    reverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const override;

    /// Determine if the branch target is in range.
    bool isBranchOffsetInRange(unsigned BranchOpc, int64_t Offset) const override;

    void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                    const DebugLoc &DL, MCRegister DestReg, MCRegister SrcReg,
                    bool KillSrc) const override;

    void storeRegToStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MBBI, 
                            Register SrcReg, bool isKill, int FrameIndex,
                            const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI,
                            Register VReg) const override;

    void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MBBI, 
                            Register DestReg, int FrameIndex, 
                            const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI,
                            Register VReg) const override;

    Register getGlobalBaseReg(MachineFunction *MF) const;

    /// GetInstSize - Return the number of bytes of code the specified
    /// instruction may be.  This returns the maximum number of bytes.
    unsigned getInstSizeInBytes(const MachineInstr &MI) const override;

    // Lower pseudo instructions after register allocation.
    bool expandPostRAPseudo(MachineInstr &MI) const override;
    */
}; // end LC3InstrInfo class

} // end namespace llvm

#endif
