add_llvm_component_group(LC3)

set(LLVM_TARGET_DEFINITIONS LC3.td)

# Generate include (.inc) file from the table_decription (.td)
tablegen(LLVM LC3GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM LC3GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM LC3GenSubtargetInfo.inc -gen-subtarget)
# tablegen(LLVM LC3GenMCPseudoLowering.inc -gen-pseudo-lowering)

add_public_tablegen_target(LC3CommonTableGen)

# LC3CodeGen should match with LLVMBuild.txt LC3CodeGen
add_llvm_target(LC3CodeGen
#  LC3AsmPrinter.cpp
  LC3RegisterInfo.cpp
  LC3InstrInfo.cpp
  LC3Subtarget.cpp
  LC3TargetMachine.cpp
  # LC3MachineFunctionInfo.cpp

  LINK_COMPONENTS
  Analysis
  # AsmPrinter
  CodeGen
  Core
  MC
  LC3Desc
  LC3Info
  SelectionDAG
  Support
  Target
  GlobalISel

  ADD_TO_COMPONENT
  LC3
  )

# Should match with "subdirectories =  MCTargetDesc TargetInfo" in LLVMBuild.txt
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
