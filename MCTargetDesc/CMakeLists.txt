add_llvm_component_library(LLVMLC3Desc
  # LC3AsmBackend.cpp
  # LC3ELFObjectWriter.cpp
  # LC3InstPrinter.cpp
  # LC3MCAsmInfo.cpp
  # LC3MCCodeEmitter.cpp
  LC3MCTargetDesc.cpp
  # LC3MCExpr.cpp
  # LC3TargetStreamer.cpp

  LINK_COMPONENTS
  MC
  # LC3Info
  Support
  # TargetParser

  ADD_TO_COMPONENT
  LC3
  )
