#define DEBUG_TYPE "buildCFG"
#include "llvm/Support/raw_ostream.h" 
#include "llvm/Support/CommandLine.h" 
#include "programCFG.h" 
#include "convinent.h"
#include <vector>

using namespace llvm;
using namespace std;
//command line argument int n
//cl::opt<int >
//subpathLength("n",  
//  cl::desc("Subpath length"), cl::value_desc("length"));
cl::opt<string>
targetListFileName("targetList",
	cl::desc("target list file name"),cl::value_desc("fileName"));
namespace {
  // buildCFG - The first implementation, without getAnalysisUsage.
	struct buildCFG : public FunctionPass {
		static char ID; // Pass identification, replacement for typeid
		buildCFG() : FunctionPass(ID) {}
		bool doInitialization(Module &M);
		virtual bool runOnFunction(Function &F) ;
		virtual void getAnalysisUsage(AnalysisUsage &AU) const {
	     		AU.setPreservesCFG();
	    	}
	  };
}

bool buildCFG::doInitialization(Module &M){ 

	new ProgramCFG(M);
 
 return false;
  
}

bool buildCFG::runOnFunction(Function &F) {   
	return false;
}
char buildCFG::ID = 0;
static RegisterPass<buildCFG> X("buildCFG", "build program CFG");


/*

*/
