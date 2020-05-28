#include "ControlFlow.h"

#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Operator.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/DebugInfoMetadata.h> 
#include <llvm/Support/SourceMgr.h>

using namespace llvm;
using namespace std;

using FuncSet = std::unordered_set<llvm::Function*>;
using CallersMap = DenseMap<llvm::StringRef, FuncSet>;

//using CallersMap = unordered_map<string, FuncSet>;
static std::unordered_map<string, CallersMap> CallableObjectsMap;

int main(int argc, char* argv[])
{
  LLVMContext context;
  SMDiagnostic error;
  //std::unique_ptr<Module> module = parseIRFile("test_code/simple-process.bc", error, context);
  //std::unique_ptr<Module> m = parseIRFile("/tmp/mlta_test/a.bc", error, context);
  std::unique_ptr<Module> module = parseIRFile("test_global/module2.bc", error, context);
  std::unique_ptr<Module> module2 = parseIRFile("test_global/module1.bc", error, context);

  if (module2) {
    for (Module::global_iterator gi = module2->global_begin();
	gi!=module2->global_end(); ++gi)  {
      GlobalVariable *GV = &*gi; 
      errs() << "gv from module1: " << GV->getGUID() << "\n";
    }
  }

  if (module)
  {
    std::queue<Function*> funcQueue;
    for (Function &func: *module) {
      funcQueue.push(&func);
    }
    for (Function &func: *module2)
      funcQueue.push(&func);

    while(!funcQueue.empty()) {
      Function *func = funcQueue.front();
      errs() << "Function call inside " << func->getName() << "\n";
      if (func->hasAddressTaken()) {
        errs() << "Address taken function!\n" ;

        DISubprogram *sp = func->getSubprogram();
        if(sp)
          errs() << "SubProg:" <<  sp->getFilename() << "\n";
      }

      for (BasicBlock &block: *func) {
	errs() << block;
        //block.print(errs(), 0);
        for (Instruction &it: block) {
          if (CallInst *ci = dyn_cast<CallInst>(&it)) {
            if (ci->isIndirectCall()) {
              //ci->print(errs());
              //for (auto it=ci->data_operands_begin(), ie=ci->data_operands_end(); it != ie; ++it)
                //errs() << (*it).get()->getType()->getTypeID() << "\n";
              //errs() << "Indirect call: " << ci->getCalledValue()->getName() << "\n";
              //errs() << "Indirect call: " << ci->getCalledValue()->stripPointerCasts()->getName() << "\n";
              //errs() << "Indirect call: " << ci->getCalledValue()->stripPointerCasts() << "\n";
	      errs() << "Indirect call from function: " << ci->getFunction()->getName() << "\n";
            } else {
              errs() << "Direct Call:" << ci << " " << ci->getCalledFunction()->getName() << "\n";

            }
            //errs() << ci->getCalledFunction()->getName() << "\n";
            //string keyName = ci->getCalledFunction()->getName();
          }

	  /*
	  if (LoadInst *ld = dyn_cast<LoadInst>(&it)) {
	    errs() << "load now!\n"; 
	    Value *PO = ld->getPointerOperand();

	    if (GlobalVariable *GV = dyn_cast<GlobalVariable>(PO))
	      errs() << "GV: " << GV->getGUID() << "\n";
	    else if (BitCastOperator *BCO = dyn_cast<BitCastOperator>(PO)) {
	      if (GlobalVariable *GV = dyn_cast<GlobalVariable>(BCO->getOperand(0)))    
	        errs() << "BCOGV: " << GV->getGUID() << "\n";
	    }
	     
	  }
	  */
        }
      }

      funcQueue.pop();
    }
  }

  return 0;
}

