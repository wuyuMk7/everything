#include <iostream>
#include <queue>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/SourceMgr.h>

using namespace llvm;

int main(int argc, char* argv[])
{
  LLVMContext context;
  SMDiagnostic error;
  std::unique_ptr<Module> module = parseIRFile("test_code/simple-process.bc", error, context);
  //std::unique_ptr<Module> m = parseIRFile("/tmp/mlta_test/a.bc", error, context);

  if (module)
  {
    std::queue<Function*> funcQueue;
    for (Function &func: *module) {
      funcQueue.push(&func);
    }

    while(!funcQueue.empty()) {
      Function *func = funcQueue.front();
      errs() << "Function call inside " << func->getName() << "\n";
      if (func->hasAddressTaken()) {
        errs() << "Address taken function!\n" ;
      }
      for (BasicBlock &block: *func) {
        //block.print(errs(), 0);
        for (Instruction &it: block) {
          if (CallInst *ci = dyn_cast<CallInst>(&it)) {
            if (ci->isIndirectCall()) {
              //ci->print(errs());
              //for (auto it=ci->data_operands_begin(), ie=ci->data_operands_end(); it != ie; ++it)
                //errs() << (*it).get()->getType()->getTypeID() << "\n";
              //errs() << "Indirect call: " << ci->getCalledValue()->getName() << "\n";
              //errs() << "Indirect call: " << ci->getCalledValue()->stripPointerCasts()->getName() << "\n";
              errs() << "Indirect call: " << ci->getCalledValue()->stripPointerCasts() << "\n";
            } else {
              errs() << "Direct Call:" << ci->getCalledFunction()->getName() << "\n";
            }
            //errs() << ci->getCalledFunction()->getName() << "\n";
          }
        }
      }

      funcQueue.pop();
    }
  }

  return 0;
}
