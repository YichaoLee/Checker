#ifndef CONVINENT_H
#define CONVINENT_H
#include <sstream>
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/Pass.h"  
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/LLVMContext.h" 
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Module.h"
#include <string>
using namespace std;
using namespace llvm;

Constant* getGlobalString(string s, Module &M);

Constant* getLocation(Instruction* I,Module &M);

string getSourceFileName(Function *f);

int getLineNumber(Instruction* I);

string getFilename(Instruction* I);
#endif