DOUBLE CHECK
* CMakeLists: allow to generate ISel.include file
* Make sure to include llvm/include/<AnyNeededFile> in mytarget files.
*

-

FAILED: lib/Target/LC3/CMakeFiles/LLVMLC3CodeGen.dir/LC3RegisterInfo.cpp.o 
...
.../LC3/LC3RegisterInfo.cpp
In file included from /home/ilsoo/myproject/trunk/llvm/lib/Target/LC3/LC3ISelLowering.h:18,
                 from /home/ilsoo/myproject/trunk/llvm/lib/Target/LC3/LC3Subtarget.h:17,
                 from /home/ilsoo/myproject/trunk/llvm/lib/Target/LC3/LC3RegisterInfo.cpp:15:
/home/ilsoo/myproject/trunk/llvm/lib/Target/LC3/LC3TargetMachine.h:50:23: error: invalid covariant return type for ‘virtual const llvm::LC3Subtarget* llvm::LC3TargetMachine::getSubtargetImpl(const llvm::Function&) const’
   50 |   const LC3Subtarget *getSubtargetImpl(const Function &F) const override;

FIX 
   Remove #include "LC3TargetMachine.h" in RegisterInfo_cpp
   Building RegisterInfo, Subtarget is not ready?
