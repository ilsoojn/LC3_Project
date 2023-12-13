add_llvm_component_library(LLVMLC3Info
  LC3TargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  LC3
  )
