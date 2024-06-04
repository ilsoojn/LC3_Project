//===-- LC3ISelLowering.cpp - LC3 DAG Lowering Implementation ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the interfaces that LC3 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

// #include "MCTargetDesc/LC3MCExpr.h"
#include "LC3ISelLowering.h"
#include "LC3MachineFunctionInfo.h"
#include "LC3RegisterInfo.h"
#include "LC3TargetMachine.h"
#include "LC3TargetObjectFile.h"
#include "MCTargetDesc/LC3MCTargetDesc.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/ISDOpcodes.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/KnownBits.h"

using namespace llvm;

#define DEBUG_TYPE "isellowering"

//FROM llvm/CodeGen/TargetLowering.h

//===----------------------------------------------------------------------===//
// TargetLowering Implementation
//===----------------------------------------------------------------------===//

LC3TargetLowering::LC3TargetLowering(const TargetMachine &TM,
                                         const LC3Subtarget &STI)
    : TargetLowering(TM), Subtarget(&STI) {
  MVT PtrVT = MVT::getIntegerVT(TM.getPointerSizeInBits(0));
  // TODO
  //=== TargetLowering Configuration Methods ===//

  setBooleanContents(ZeroOrOneBooleanContent);
  setMinFunctionAlignment(Align(2));

  // Register
  computeRegisterProperties(STI.getRegisterInfo());
  addRegisterClass({MVT::i8, MVT::i16}, &LC3::GPRRegClassID);
  setStackPointerRegisterToSaveRestore(LC3::SP);

  // General Operations
  // Arithmetics :: LC3 does not have SUB, MUL, DIV/REM (has ADD)
  setOperationAction(ISD::ADD, {MVT::i8, MVT::i16}, Custom);
  setOperationAction(ISD::SUB, {MVT::i8, MVT::i16}, Custom);
  setOperationAction(ISD::MUL, {MVT::i8, MVT::i16}, Custom);
  setOperationAction({ISD::SDIV, ISD::UDIV}, {MVT::i8, MVT::i16}, Custom);
  setOperationAction({ISD::SREM, ISD::UREM}, {MVT::i8, MVT::i16}, Custom);
  setOperationAction({ISD::SDIVREM, ISD::UDIVREM}, {MVT::i8, MVT::i16}, Custom);

  // Logic :: LC3 does not have OR, XOR, SHA, SHL, SHR (has AND)
  setOperationAction(ISD::AND, {MVT::i8, MVT::i16}, Custom);
  setOperationAction(ISD::OR, {MVT::i8, MVT::i16}, Custom);
  setOperationAction(ISD::XOR, {MVT::i8, MVT::i16}, Custom);
  setOperationAction(ISD::SRA, {MVT::i8, MVT::i16}, Custom);
  setOperationAction(ISD::SHL, {MVT::i8, MVT::i16}, Custom);
  setOperationAction(ISD::SRL, {MVT::i8, MVT::i16}, Custom);
  // setOperationAction(ISD::NOT, {MVT::i8, MVT::i16}, Custom);  // NOT

  // Branch/Jump :: LC3 does not have BR_CC, BRIND, BR_JT (use BRCOND)
  setOperationAction(ISD::BR_CC, {MVT::i8, MVT::i16}, Expand);
  setOperationAction(ISD::BRIND, {MVT::i8, MVT::i16}, Expand);
  setOperationAction(ISD::BR_JT, {MVT::i8, MVT::i16}, Expand);
  setOperationAction(ISD::BRCOND, {MVT::i8, MVT::i16}, Custom);      // BRNZP
  setOperationAction(ISD::BR, {MVT::i8, MVT::i16}, Custom);          // JMP, JSR, JSRR
  setOperationAction(ISD::RETURNADDR, {MVT::i8, MVT::i16}, Custom);  // RET, RTI

  // Load/Store
  setOperationAction(ISD::LOAD, {MVT::i8, MVT::i16}, Custom);        // LD, LDR, LDI, LEA
  setOperationAction(ISD::STORE, {MVT::i8, MVT::i16}, Custom);       // ST, STR, STI
  
  // Trap
  setOperationAction(ISD::TRAP, {MVT::i8, MVT::i16}, Custom);  // TRAP
}

SDValue LC3TargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {

  switch (Op.getOpcode()) {
  default: llvm_unreachable(("Should not custom lower this " + Op->getOperationName()).c_str());

  // Arithmetics
  case ISD::ADD:
  case ISD::SUB:
  case ISD::MUL:
  case ISD::SDIV:
  case ISD::UDIV:
  case ISD::SREM:
  case ISD::UREM:
  case ISD::SDIVREM:
  case ISD::UDIVREM:
    return LowerARITHMETIC(Op, DAG);

  // Logicals
  case ISD::AND:
  case ISD::OR:
  case ISD::XOR:
  case ISD::SRA:
  case ISD::SRL:
  case ISD::SHL:
    return LowerLOGICAL(Op, DAG);
  
  case ISD::BR:                 return LowerBR(Op, DAG, *this);
  case ISD::BRCOND:             return LowerBRCOND(Op, DAG, *this);
  
  case ISD::RETURNADDR:         return LowerRETURNADDR(Op, DAG, *this, Subtarget);
  case ISD::FRAMEADDR:          return LowerFRAMEADDR(Op, DAG, Subtarget);
  
  // case ISD::TRAP:               return LowerTRAP(Op, DAG);
  // case ISD::LOAD:               return LowerLOAD(Op, DAG);
  // case ISD::STORE:              return LowerSTORE(Op, DAG);
  }
}

/// Replace a node with an illegal result type
/// with a new node built out of custom code.
void LC3TargetLowering::ReplaceNodeResults(SDNode *N,
                                           SmallVectorImpl<SDValue> &Results,
                                           SelectionDAG &DAG) const {
}

const char *LC3TargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch ((LC3ISD::NodeType)Opcode) {
  case LC3ISD::FIRST_NUMBER:    break;
  case LC3ISD::BRNZP:           return "LC3ISD::BRNZP";
  case LC3ISD::JUMP:            return "LC3ISD::JUMP";
  case LC3ISD::JMPSUB:          return "LC3ISD::JMPSUB";
  case LC3ISD::NOT:             return "LC3ISD::NOT";
  }
  return nullptr;
}

EVT LC3TargetLowering::getSetCCResultType(const DataLayout &DL, LLVMContext &Context,
                        EVT VT) const {
  assert(!VT.isVector() && "No LC3_SetCC for the vector");
  return MVT::i8;
}

Register LC3TargetLowering::getRegisterByName(const char* RegName, LLT Ty,const MachineFunction &MF) const {
  Register Reg = StringSwitch<Register>(RegName)
    .Case("R0", LC3::R0).Case("R1", LC3::R1)
    .Case("R2", LC3::R2).Case("R3", LC3::R3).Case("R4", LC3::R4)
    .Case("R5", LC3::FP).Case("R6", LC3::SP).Case("R7", LC3::LR)
    .Case("PC", LC3::PC).Case("SR", LC3::SR)
    .Default(0);

  if (Reg)
    return Reg;

  report_fatal_error(Twine("Invalid register name \"" + StringRef(RegName) + "\"."));
}

/*
// getPre(Post)IndexedAddressParts
// LD R0, [R1]              :::   R0 <- [R1]
// Offset  LD R0, [R1, #8]  :::   R0 <- [R1 + #8]
// PreIdx  LD R0, #8, [R1]  :::   R1 <- R1 + #8, R0 <- [R1] ::: r1 is modified
// PostIdx LD R0, [R1], #8  :::   R0 <- [R1], R1 <- R1 + #8
//
// Question: This is from Mips & RISCV
bool isLegalAddressingMode(const DataLayout &DL, const AddrMode &AM, 
                          Type *Ty, unsigned AS,
                          Instruction *I = nullptr) const {
   // No global is ever allowed as a base.
  if (AM.BaseGV)
    return false;

  switch (AM.Scale) {
  case 0: // "r+i" or just "i", depending on HasBaseReg.
    break;
  case 1:
    if (!AM.HasBaseReg) // allow "r+i".
      break;
    return false; // disallow "r+r" or "r+r+i".
  default:
    return false;
  }

  return true;
}
// QuestionEnd
*/

//===----------------------------------------------------------------------===//
//                     Lower Operator Implementation
//===----------------------------------------------------------------------===//

SDValue LC3TargetLowering::LowerARITHMETIC(SDValue Op, SelectionDAG &DAG) const {
}
SDValue LC3TargetLowering::LowerLOGICAL(SDValue Op, SelectionDAG &DAG) const {
}

SDValue LC3TargetLowering::LowerRETURNADDR(SDValue Op, SelectionDAG &DAG, 
                                          const LC3TargetLowering &TLI, 
                                          const LC3Subtarget *Subtarget) const {
  
  if (TLI.verifyReturnAddressArgumentIsConstant(Op, DAG))
    return SDValue();

  // check the depth
  assert((cast<ConstantSDNode>(Op.getOperand(0))->getZExtValue() == 0) &&
         "Return address can be determined only for current frame.");

  // get current ReturnOp function

  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  MVT VT = Op.getSimpleValueType();
  unsigned LR = LC3::LR; //LC3_R7
  MFI.setReturnAddressIsTaken(true);

  // Return LR, which contains the return address. Mark it an implicit live-in.
  unsigned Reg = MF.addLiveIn(LR, getRegClassFor(VT));
  return DAG.getCopyFromReg(DAG.getEntryNode(), SDLoc(Op), Reg, VT);

}
SDValue LC3TargetLowering::LowerFRAMEADDR(SDValue Op, SelectionDAG &DAG, const LC3Subtarget *Subtarget) const {
}

SDValue LC3TargetLowering::LowerBR(SDValue Op, SelectionDAG &DAG, const LC3TargetLowering &TLI) const {
}
SDValue LC3TargetLowering::LowerBRCOND(SDValue Op, SelectionDAG &DAG, const LC3TargetLowering &TLI) const {
}

SDValue LC3TargetLowering::LowerLOAD(SDValue Op, SelectionDAG &DAG) const {
}
SDValue LC3TargetLowering::LowerSTORE(SDValue Op, SelectionDAG &DAG) const {
}

SDValue LC3TargetLowering::LowerTRAP(SDValue Op, SelectionDAG &DAG) const {
}
//===----------------------------------------------------------------------===//
//                     Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "LC3GenCallingConv.inc"

SDValue LC3TargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                     SmallVectorImpl<SDValue> &InVals) const {
}

/// Lower the result values of a call into the
/// appropriate copies out of appropriate physical registers.
///
SDValue LC3TargetLowering::LowerCallResult(
    SDValue Chain, SDValue InGlue, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &dl,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
}

bool LC3TargetLowering::CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF,
                        bool isVarArg,
                        const SmallVectorImpl<ISD::OutputArg> &Outs,
                        LLVMContext &Context) const {
                        
                        }

SDValue LC3TargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
                        const SmallVectorImpl<ISD::OutputArg> &Outs,
                        const SmallVectorImpl<SDValue> &OutVals,
                        const SDLoc &dl, SelectionDAG &DAG) const {

                        }

//===----------------------------------------------------------------------===//
//                         LC3 Inline Assembly Support
//===----------------------------------------------------------------------===//

