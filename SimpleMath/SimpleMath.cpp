#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Analysis/ConstantFolding.h"
#include<iterator> // for iterators
#include <string>

using namespace llvm;

namespace {
struct SimpleMath : public FunctionPass {
  static char ID;
  SimpleMath() : FunctionPass(ID) {}

  bool doInitialization(Module &M) {
    return false;
  }

  void printOperand(Value *op) {
    if (isa<Instruction>(*op)) {
      Instruction *_I = cast<Instruction>(op);
      errs() << *_I << "\n";
    } else if (isa<ConstantInt>(*op)) {
      ConstantInt *_V = cast<ConstantInt>(op);
      errs() << std::to_string(_V->getSExtValue()) << "\n";
    }else errs() << op << "\n";
  }

  bool runOnFunction(Function &F) {

    for (BasicBlock &BB : F) {
      // BasicBlock::iterator bi = BB.begin();
      int n = 0;
      for (BasicBlock::iterator is = BB.begin(), ie = BB.end(); is != ie; ++is){
        Instruction& I  = *is;
        Instruction* LI = &I;
        for(auto user: I.users()){
          if(isa<Instruction>(*user)){
            // errs() << "I" << n << ":" << *user << "\n";
            // errs() << n2++ << "\n";
            LI = cast<Instruction>(user);
          }
        }
        // BasicBlock::iterator(LI)
        int instIndex = 0;
        int count = 0;
        for (BasicBlock::iterator is = BB.begin(), ie = BB.end(); is != ie; ++is){
          Instruction& I  = *is;
          if(&I == LI){
            instIndex = count;
            break;
          }
          count++;
        }
        errs() << "I" << n << ": " << I << " | I" << instIndex << ": " << *LI << "\n";
        n++;
      }
    }

    for (BasicBlock &BB : F) {
      // BasicBlock::iterator bi = BB.begin();
      for (BasicBlock::iterator is = BB.begin(), ie = BB.end(); is != ie; ++is){
        Value *left;
        Value *right;
        Instruction& I  = *is;
        if(I.getOpcode() == Instruction::SDiv || I.getOpcode() == Instruction::Add || I.getOpcode() == Instruction::Mul || I.getOpcode() == Instruction::Sub){
          left = I.getOperand(0);
          right = I.getOperand(1);
          if(isa<ConstantInt>(*left) && isa<ConstantInt>(*right)){
            Constant *valor = ConstantFoldInstruction(&I, I.getModule()->getDataLayout());
            ReplaceInstWithValue(BB.getInstList(), is, valor);

          }

        }
      }
    }
    return false;
  }

  bool doFinalization(Module &M) {
    return false;
  }
};
}

char SimpleMath::ID = 0;
static RegisterPass<SimpleMath> X("sm", "Simple math pass", false, false);
