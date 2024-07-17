//===-- LC3ISelLowering.h - LC3 DAG Lowering Interface ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that LC3 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_LC3_LC3ISELLOWERING_H
#define LLVM_LIB_TARGET_LC3_LC3ISELLOWERING_H

#include "LC3.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {
  class LC3Subtarget;

  namespace LC3ISD {
  enum NodeType : unsigned {
    FIRST_NUMBER = ISD::BUILTIN_OP_END,

    BRNZP,
    JUMP,
    JMPSUB,
    NOT

  };
  }

class LC3TargetMachine;

  class LC3TargetLowering : public TargetLowering {
    const LC3Subtarget *Subtarget;

  public:
    LC3TargetLowering(const TargetMachine &TM, const LC3Subtarget &STI);
    SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

    void ReplaceNodeResults(SDNode *N, SmallVectorImpl<SDValue> &Results,
                          SelectionDAG &DAG) const override;

    /// getTargetNodeName - This method returns the name of a target specific DAG node.    
    const char *getTargetNodeName(unsigned Opcode) const override;

    /// getSetCCResultType - Return the ISD::SETCC ValueType
    EVT getSetCCResultType(const DataLayout &DL, LLVMContext &Context,
                           EVT VT) const override;

    /// Return the register ID of the name passed in. Used by named register
    /// global variables extension. There is no target-independent behaviour
    /// so the default action is to bail.
    Register getRegisterByName(const char* RegName, LLT Ty,const MachineFunction &MF) const override;
  
    /// Return true if the addressing mode represented by AM is legal for this
    /// target, for a load/store of the specified type.
    ///
    /// The type may be VoidTy, in which case only return true if the addressing
    /// mode is legal for a load/store of any legal type.  TODO: Handle
    /// pre/postinc as well.
    ///
    /// If the address space cannot be determined, it will be -1.
    ///
    /// TODO: Remove default argument
    // bool isLegalAddressingMode(const DataLayout &DL, const AddrMode &AM, 
    //                           Type *Ty, unsigned AS,
    //                           Instruction *I = nullptr) const override;

    /// Returns true if the specified base+offset is a legal indexed addressing
    /// mode for this target. \p MI is the load or store instruction that is being
    /// considered for transformation.
    // virtual bool isIndexingLegal(MachineInstr &MI, Register Base, Register Offset,
    //                             bool IsPre, MachineRegisterInfo &MRI) const override;

/*
    /// Return true if folding a constant offset with the given GlobalAddress is
    /// legal.  It is frequently not legal in PIC relocation models.
    virtual bool isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const {
      return true;
    };
*/

    //===----------------------------------------------------------------------===//
    //                     Calling Convention Implementation
    //===----------------------------------------------------------------------===//

    SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                        SmallVectorImpl<SDValue> &InVals) const override;

    /// Lower the result values of a call into the
    /// appropriate copies out of appropriate physical registers.
    ///
    SDValue LowerCallResult(
        SDValue Chain, SDValue InGlue, CallingConv::ID CallConv, bool isVarArg,
        const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &dl,
        SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const;

    bool CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF,
                            bool isVarArg,
                            const SmallVectorImpl<ISD::OutputArg> &Outs,
                            LLVMContext &Context) const override;

    SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
                            const SmallVectorImpl<ISD::OutputArg> &Outs,
                            const SmallVectorImpl<SDValue> &OutVals,
                            const SDLoc &dl, SelectionDAG &DAG) const override;


    private:
    // LowerOperation functions
    
    //===----------------------------------------------------------------------===//
    //                     Lower Operator Implementation
    //===----------------------------------------------------------------------===//

    SDValue LowerARITHMETIC(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerLOGICAL(SDValue Op, SelectionDAG &DAG) const;

    SDValue LowerRETURNADDR(SDValue Op, SelectionDAG &DAG, const LC3TargetLowering &TLI, const LC3Subtarget *Subtarget) const;
    SDValue LowerFRAMEADDR(SDValue Op, SelectionDAG &DAG, const LC3Subtarget *Subtarget) const;

    SDValue LowerBR(SDValue Op, SelectionDAG &DAG, const LC3TargetLowering &TLI) const;
    SDValue LowerBRCOND(SDValue Op, SelectionDAG &DAG, const LC3TargetLowering &TLI) const;

    SDValue LowerLOAD(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerSTORE(SDValue Op, SelectionDAG &DAG) const;

    SDValue LowerTRAP(SDValue Op, SelectionDAG &DAG) const;

  };
} // end namespace llvm

#endif // LLVM_LIB_TARGET_LC3_LC3ISELLOWERING_H
