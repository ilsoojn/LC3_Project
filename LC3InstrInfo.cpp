//===-- LC3InstrInfo.cpp - LC3 Instruction Information ----------------===//
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

#include "LC3InstrInfo.h"
#include "LC3.h"
#include "LC3MachineFunctionInfo.h"
#include "LC3Subtarget.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "LC3GenInstrInfo.inc"

namespace llvm {

class LC3InstrInfo : public LC3GenInstrInfo {
    const LC3RegisterInfo RI;
//   const LC3Subtarget& Subtarget;
public:
    explicit LC3InstrInfo(LC3Subtarget &STI);

    /// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
    /// such, whenever a client has an instance of instruction info, it should
    /// always be able to get register info as well (through this method).
    ///
    const LC3RegisterInfo &getRegisterInfo() const { return RI; }

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
                                int &FrameIndex) const {
    }

    MachineBasicBlock *getBranchDestBlock(const MachineInstr &MI) const {
    }

    bool analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
                        MachineBasicBlock *&FBB,
                        SmallVectorImpl<MachineOperand> &Cond,
                        bool AllowModify = false) const {
    }

    unsigned removeBranch(MachineBasicBlock &MBB,
                        int *BytesRemoved = nullptr) const {
    }

    unsigned insertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                        MachineBasicBlock *FBB, ArrayRef<MachineOperand> Cond,
                        const DebugLoc &DL,
                        int *BytesAdded = nullptr) const {
    }

    bool
    reverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const {
    }

    /// Determine if the branch target is in range.
    bool isBranchOffsetInRange(unsigned BranchOpc, int64_t Offset) const {
    }

    void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                    const DebugLoc &DL, MCRegister DestReg, MCRegister SrcReg,
                    bool KillSrc) const {
    }

    void storeRegToStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MBBI, 
                            Register SrcReg, bool isKill, int FrameIndex,
                            const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI,
                            Register VReg) const {
    }

    void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MBBI, 
                            Register DestReg, int FrameIndex, 
                            const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI,
                            Register VReg) const {
    }

    Register getGlobalBaseReg(MachineFunction *MF) const;

    /// GetInstSize - Return the number of bytes of code the specified
    /// instruction may be.  This returns the maximum number of bytes.
    unsigned getInstSizeInBytes(const MachineInstr &MI) const {
    }

    // Lower pseudo instructions after register allocation.
    bool expandPostRAPseudo(MachineInstr &MI) const {
    }
};

}
