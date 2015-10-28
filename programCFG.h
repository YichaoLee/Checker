#ifndef PROGRAMCFG_H
#define PROGRAMCFG_H
#include "llvm/IR/BasicBlock.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/IR/Metadata.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h" 
#include <vector> 
#include "convinent.h"
#include "CFG.h"
#include "InstParser.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/User.h"
//#include "../lib/IR/AsmWriter.h"
#include "llvm/IR/DebugInfo.h"
#include "Verification.h"
using namespace llvm;
using namespace std;
struct CFGNode;

//a succ link list
struct Succ{
    CFGNode *v;
    Succ *nextSucc;
    Succ(CFGNode* newNode):v(newNode),nextSucc(NULL){}
};

//a prev link list
struct Prev{
    CFGNode *v;
    Prev *nextPrev;
    Prev(CFGNode* newNode):v(newNode),nextPrev(NULL){}
};

struct CFGNode{
    BasicBlock *bb; 
    Succ* firstSucc; 
    Prev* firstPrev;
    string name;
    int id;//id == the 

    static int counter_state;//count State to assign id.
    static int counter_variable;//count Varibale to assign id.
    

    CFGNode(){
        CFGNode(NULL, NULL);
    }

    CFGNode (BasicBlock *b, CFG *cfg):bb(b),firstSucc(NULL),firstPrev(NULL) {  
        if(b){

            stringstream ost;
            ost<<counter_state;
            string  str;
            ost>>str;
           
            //Generate a new state
            int id = counter_state;
            name = "s"+str;
            State* s = new State(false, id, name);
            raw_ostream &ROS = errs();
            formatted_raw_ostream OS(ROS);
            global_CFG.InsertCFGState(counter_state,name);
             
	    
	    const Function* F = b->getParent()?b->getParent():nullptr;
	    string func = F->getName();
	    if(func=="main" && b==F->begin()){
		cfg->initialState=s;
		s->isInitial=true;
//		errs()<<"cfg initialed:"<<s->name<<"\n";
            }
            //Generate the new vector<Constraint>
            BasicBlock::iterator it_end = b->end();
            for(BasicBlock::iterator it = b->begin(); it != it_end; it ++){
                const Instruction* I = dyn_cast<Instruction>(it);
//                const Function* F = I->getParent()?I->getParent()->getParent():nullptr;
		SlotTracker SlotTable(F);
                const Module* M = F?F->getParent():nullptr;
                InstParser W(OS, SlotTable, M, nullptr);
//		errs()<<"InL:\n";W.printInstructionLine(*I);
                //create the LabelMap
                if(it == b->begin())
                    W.InsertCFGLabel(cfg,b,s, func); 
                //create the variable table
                counter_variable = W.setVariable(cfg, I, counter_variable, func);
                //create the constraint table(NOT TRANSITION)
//                counter_state = W.setConstraint(cfg, s, I, func, counter_state);
            } 
            cfg->stateList.push_back(*s);
	    
        }else {
            ;
        }
        id = counter_state++;
    }

    void addSucc(CFGNode *succ){
        assert(succ && succ->bb);
        Succ *newSucc = new Succ(succ);
        if(firstSucc == NULL){
            firstSucc = newSucc; 
        }else{
            newSucc->nextSucc = firstSucc;
            firstSucc = newSucc;
        }
        Transition* tmp = new Transition(name,newSucc->v->name);
        global_CFG.InsertCFGTransition(tmp);
//        errs()<<"transition id(name):"<<tmp->tran_id<<"\t"<<name<<"\t"<<newSucc->v->name<<"...\n";
        global_CFG.getState(this->id)->InsertTransition(tmp);

    }


    void addPrev(CFGNode *prev){
        assert(prev && prev->bb);
        Prev *newPrev = new Prev(prev);
        if(firstPrev == NULL){
            firstPrev = newPrev; 
        }else{
            newPrev->nextPrev = firstPrev;
            firstPrev = newPrev;
        }
    }

    Succ *getFirstSucc(){
        return firstSucc;
    }

    Prev *getFirstPrev(){
        return firstPrev;
    }

    bool operator == (const CFGNode &node){ 
        return  (bb == node.bb) ;
    }

    template<class Stream>
        friend Stream& operator << (Stream &outputStream,const CFGNode *node){
            if(node){
                outputStream << node->id; 
            }
            return outputStream;
        }
};



class ProgramCFG{
    private:
        void initTargetFunctionList(Module &m);
        void findAllRetBlocks(Module &m);
        void buildProgramCFG(Module &m, CFG *cfg);
	void readFunc(string funcName, CFG *cfg, int time);
	void readBasicblock(BasicBlock *b, CFG *cfg, int time);
        void createSucc(BasicBlock *v);
        void bfs(CFGNode *v);
        CFGNode* root;
        Module &M;
    public:
        ProgramCFG(Module &m);
        std::map<BasicBlock*, CFGNode> *nodes;
        CFGNode *getRoot(){
            return root;
        } 

        CFGNode *getNode(BasicBlock* bb){
            return &((*nodes)[bb]);
        }

};



#endif
