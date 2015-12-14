#include "programCFG.h"
#include "mylib/printSTL.h"
#include "llvm/Support/FileSystem.h"
#include "CFGWriter.h"
#include <stack>  
#include "llvm/Support/CommandLine.h"
#include <time.h>
using namespace std;
using namespace llvm;
//std::map<BasicBlock*, CFGNode>* ProgramCFG::nodes = NULL;
int CFGNode::counter_state = 0;
int CFGNode::counter_variable = 0;
static int counter_state = 0;
static int counter_variable = 0;


//Command line argument
cl::opt<int>
lineNo("line",
        cl::desc("Line Number"), cl::value_desc("line"));
cl::opt<string>
filename("name",
        cl::desc("File Name"), cl::value_desc("name"));
cl::opt<int>
bound("bound",
        cl::desc("check bound"), cl::value_desc("check bound"));
cl::opt<string>
check("check",
        cl::desc("check"), cl::value_desc("check"));


bool if_a(char x){
    if((x<='z' && x>='a')||(x<='Z' && x>='A'))
        return true;
    else 
        return false;
}

bool if_num(char x){
    if(x<='9' && x>='0')
        return true;
    else 
        return false;
}

bool if_op(char x){
    if(x=='=' || x=='<' || x=='>'|| x=='!')
        return true;
    else 
        return false;
}

bool if_as(char x){
    if(x=='+' || x=='-'|| x=='*'|| x=='/')
        return true;
    else
        return false;
}

string convertToString(double d) {  
 ostringstream os;  
 if (os << d)  
  return os.str();  
 return "invalid conversion";  
}

CFG *Add_target(CFG *cfg,int target,string check){
    CFG *cfg1=new CFG();
    cfg1=cfg;
    int len=check.length();
    char x[len];
    int numCheck=1;//budengshi geshu
    for(int i=0;i<len;i++){
        x[i]=check.at(i);
        if(x[i]=='&')
            numCheck++;
    }
    State *old_target=cfg->searchState(target);
    State *new_target=new State(false,-1,"q1");
    new_target->transList.clear();
    new_target->consList.clear();
    Transition *temp=new Transition(-2,"p1");
    temp->fromState=old_target;
    temp->fromName=cfg->getNodeName(target);
    temp->toState=new_target;
    temp->toName="q1";
    temp->guardList.clear();
    int a[numCheck+1];
    int k=0;
    a[k++]=0;
    for(int i=0;i<len;i++)
        if(x[i]=='&')
            a[k++]=i+1;
    a[numCheck]=len+1;
    string b[numCheck];
    for(int i=0;i<numCheck;i++)
        b[i]=check.substr(a[i],a[i+1]-a[i]-1);
/*
    Constraint ctemp;
    ParaVariable ptemp;
    for(int i=0;i<numCheck;i++){
//      ctemp.lpvList.clear();
//      ctemp.rpvList.clear();
        int opl=0;
        int ltemp=a[i+1]-a[i]-1;
        char c[ltemp];
        for(int m=0;m<ltemp;m++){
            c[m]=b[i].at(m);
        }
        for(int m=0;m<ltemp;m++){
            if(if_op(c[m])){
                opl=m;
                break;
            }
        }  
        int judgeOp=1;
        if(c[opl]=='!' && c[opl+1]=='='){
            ctemp.op=NE;
            judgeOp=1;
        }
        if(c[opl]=='<' && c[opl+1]=='='){
            ctemp.op=LE;
            judgeOp=1;
        }
        if(c[opl]=='>' && c[opl+1]=='='){
            ctemp.op=GE;
            judgeOp=1;
        }
        if(c[opl]=='<' && !if_op(c[opl+1])){
            ctemp.op=LT;
            judgeOp=0;
        }
        if(c[opl]=='>' && !if_op(c[opl+1])){
            ctemp.op=GT;
            judgeOp=0;
        }
        if(c[opl]=='=' && !if_op(c[opl+1])){
            ctemp.op=EQ;
            judgeOp=0;
        }
        if(c[opl]=='=' && c[opl+1]=='='){
            ctemp.op=EQ;
            judgeOp=0;
        }

        string t1,t2;
        t1=b[i].substr(0,opl);
        t2=b[i].substr(opl+judgeOp+1,ltemp-opl-judgeOp-1);
        //errs()<<"t1="<<t1<<";t2="<<t2<<"\n";
        //errs()<<"letmp="<<ltemp<<";opl="<<opl<<"\n";
        bool pIsNumber=true;string pParameter="";string vName="";
        char d1[opl];
        for(int i=0;i<opl;i++){
            d1[i]=t1.at(i);
        }
        for(int i=0;i<opl;i++){
            if(if_as(d1[i])){
                    if(i!=0){
                    ptemp.isNumber=pIsNumber;
                    if(!pIsNumber)
                        ptemp.var=cfg->getVariable(vName);
                    if(pParameter=="" ||pParameter=="-")
                        pParameter+="1";
                    if(pParameter=="+")
                        pParameter="1";
                    ptemp.parameter=pParameter;
                    ctemp.lpvList.push_back(ptemp);
                    pIsNumber=true;
                    pParameter="";
                    vName="";
                    pParameter+=d1[i];
                }
                else {
                    if(d1[i]=='-')
                        pParameter+=d1[i];
                }
            }
            if(if_num(d1[i]))
                pParameter+=d1[i];
            if(if_a(d1[i])){
                vName+=d1[i];
                pIsNumber=false;
            }
        }
        ptemp.isNumber=pIsNumber;
        if(!pIsNumber)
            ptemp.var=ptemp.var=cfg->getVariable(vName);
        if(pParameter=="" ||pParameter=="-")
            pParameter+="1";
        if(pParameter=="+")
            pParameter="1";
        ptemp.parameter=pParameter;
        ctemp.lpvList.push_back(ptemp);
    
        //errs()<<"length="<<ltemp-opl-judgeOp-1<<"\n";
        pIsNumber=true;pParameter="";vName="";
        char d2[ltemp-opl-judgeOp-1];
        for(int i=0;i<ltemp-opl-judgeOp-1;i++){
            d2[i]=t2.at(i);
        }

        for(int i=0;i<ltemp-opl-judgeOp-1;i++){
            if(if_as(d2[i])){
                    if(i!=0){
                    ptemp.isNumber=pIsNumber;
                    if(!pIsNumber)
                        ptemp.var=cfg->getVariable(vName);
                    if(pParameter=="" ||pParameter=="-")
                        pParameter+="1";
                    if(pParameter=="+")
                        pParameter="1";
                    ptemp.parameter=pParameter;
                    ctemp.rpvList.push_back(ptemp);
                    pIsNumber=true;
                    pParameter="";
                    vName="";
                    pParameter+=d2[i];
                }
                else {
                    if(d2[i]=='-')
                        pParameter+=d2[i];
                }
            }
            if(if_num(d2[i])){
                pParameter+=d2[i];
             //  errs()<<"pParameter="<<pParameter<<"\n";
            }
            if(if_a(d2[i])){
                vName+=d2[i];
                pIsNumber=false;
            }
        }
        ptemp.isNumber=pIsNumber;
        if(!pIsNumber)
            ptemp.var=ptemp.var=cfg->getVariable(vName);
        if(pParameter=="" ||pParameter=="-")
            pParameter+="1";
        if(pParameter=="+")
            pParameter="1";
        ptemp.parameter=pParameter;
        ctemp.rpvList.push_back(ptemp);
        //errs()<<"ctemp="<<ctemp<<"\n";

        temp->guardList.push_back(ctemp);
    }
*/
    cfg1->stateList.push_back(*new_target);
    cfg1->transitionList.push_back(*temp);
    return cfg1;
}
ProgramCFG::ProgramCFG(Module &m):M(m){

    root = NULL;
    nodes = NULL;

    clock_t start,finish;

    start=clock();
    errs() <<"START~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    //Create the cfg Structure
    CFG* cfg = new CFG();
    buildProgramCFG(m, cfg);
    CFG *cfg1=new CFG();
    cfg->initial();

    finish=clock();
    errs() << "BUILDCFG Time: \t" << convertToString(1000*(double)(finish-start)/CLOCKS_PER_SEC) << "ms\n";
/*
    //Draw the cfg Graph
    std::string errorInfo="";
    raw_fd_ostream File("CFG.dot",errorInfo,sys::fs::F_Text);
    if(!errorInfo.empty()){
        errs() << "can't write file CFG.dot\n";
        exit(-1); 
    }
    CFGWriter cfgWriter(this,File,true);
    cfgWriter.writeGraph1("CFG",cfg);
*/
    int inputbound=bound;
    vector<int> target;
    for(unsigned int i=0;i<cfg->stateList.size();i++){
    for(vector<int>::iterator it=cfg->stateList[i].locList.begin();it<cfg->stateList[i].locList.end();it++){
        if(*it==lineNo){
            target.push_back(cfg->stateList[i].ID);
	    errs()<<"target["<<target.size()<<"]:"<<cfg->stateList[i].name<<"\n";
            break;
        }
    }
}
    if(check=="")
    {
    //cfg->print();
    BoundedVerification verify(cfg,inputbound,target);   
    clock_t start,finish;
    start=clock();
    verify.check(lineNo,check);
    finish=clock();
    errs() << "\nbound:\t" << bound <<"\tlineNo:\t" << lineNo << "\tcheck:\t" << check << "\n";
    errs() << "Time: \t" << convertToString(1000*(double)(finish-start)/CLOCKS_PER_SEC) << "ms \n";
	ofstream SaveFile("/home/cfg/Documents/test_CFG_v3/benchmarkresult.txt", ios::app);
	SaveFile<< "\nbound:\t" << bound <<"\tlineNo:\t" << lineNo << "\tcheck:\t" << check << "\n";
    	SaveFile<< "Time: \t" << convertToString(1000*(double)(finish-start)/CLOCKS_PER_SEC) << "ms \n\n";
	SaveFile.close();
    }
    else {
    cfg1=Add_target(cfg,target[0],check);
    cfg1->initial();
    vector<int> new_target;
    for(unsigned int i=0;i<cfg1->stateList.size();i++){
        if(cfg1->stateList[i].name=="q1")
            new_target.push_back(cfg1->stateList[i].ID);
    }
    BoundedVerification verify(cfg1,inputbound,new_target);
    clock_t start,finish;
    start=clock();
    verify.check(lineNo,check);
    finish=clock();
    errs() << "bound:\t" << bound <<"\tlineNo:\t" << lineNo << "\tcheck:\t" << check << "\n";
    errs() << "Time: \t" << convertToString(1000*(double)(finish-start)/CLOCKS_PER_SEC) << "ms \n";
    }
}

enum color{WHITE,BLACK,GRAY};
std::map<BasicBlock*, color> *colors;

std::map<Function*, std::vector<BasicBlock*> > *retBlocks;
std::set<Function*> *targetFunctionList; //only use in search


void ProgramCFG::findAllRetBlocks(Module &m){
    for(Module::iterator f = m.begin(); f!= m.end(); f++){
        if(f->getName() == "main") continue;
        //if(targetFunctionList->find(f) == targetFunctionList->end()) continue;//didn't find in target func list 
        for(Function::iterator bb = f->begin(); bb != f->end(); bb++){
            TerminatorInst *terminator = bb->getTerminator();
            if(isa<ReturnInst>(terminator) ){
                //errs() <<"ret basicblock :in "<<f->getName()<<"\t"<<*terminator << "\n";
                ((*retBlocks)[f]).push_back(bb);
            }/*else if(isa<UnreachableInst>(terminator)) {
            //errs()<<"hehe: "<<f->getName() <<"\t"<<*bb<<"\n";//bb contains exit ,abort or xalloc_die
            }*/
        }
    }

    //errs() << (*retBlocks) << "\n";

}

//build program cfg in the main source file !
void  ProgramCFG::buildProgramCFG(Module &m, CFG* cfg){
    Function *main = m.getFunction("main");  
    if(main == NULL){
        errs() <<  "error: There is no main function in the Module!\n";
//        exit(-1) ;
    }

    readFunc("main", cfg, 0);
	
}

void ProgramCFG::readFunc(string funcName, CFG *cfg, int time){
	Function *F = M.getFunction(funcName);  
    	if(F == NULL){
    	    	errs() <<  "error readFunc 10086!\n";
		exit(-1) ;
	}

    	for(Function::iterator bb = F->begin(); bb != F->end(); bb++){
        	readBasicblock(bb, cfg, time);
    	}
}

void ProgramCFG::readBasicblock(BasicBlock *b, CFG *cfg, int time){
	string callFunc;
	if(b){

           
            //Generate a new state
            int id = counter_state++;
            string  str = convertToString(id);
            string name = "s"+str;
            State* s = new State(false, id, name);
            raw_ostream &ROS = errs();
            formatted_raw_ostream OS(ROS);

            global_CFG.InsertCFGState(counter_state,name);
            
	    
	    const Function* F = b->getParent()?b->getParent():nullptr;
	    string func = F->getName();
	    if(time>0)
		func = func+convertToString(time);
//	    errs()<<"0:readBasicblock "<<func<<"\n";
	    if(func=="main" && b==F->begin()){
		cfg->initialState=s;
		s->isInitial=true;
//		errs()<<"cfg initialed:"<<s->name<<"\n";
            }
	    else if(b==F->begin()){
		for (Function::const_arg_iterator it = F->arg_begin(), E = F->arg_end();
                it != E; ++it) {
			string varNum = it->getName();
			string varName = func+"_"+varNum;
			Variable var(varName, counter_variable, false);
			cfg->variableList.push_back(var);
			counter_variable ++;
		}
	    }
	    
            //Generate the new vector<Constraint>
            BasicBlock::iterator it_end = b->end();
            for(BasicBlock::iterator it = b->begin(); it != it_end; it ++){              
                const Instruction* I = dyn_cast<Instruction>(it);
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
                W.setConstraint(cfg, s, it, func);
		string op = I->getOpcodeName();
//		errs()<<"1:readBasicblock "<<func<<":"<<op<<"\n";
		if(op=="call"){
			const CallInst *call = dyn_cast<CallInst>(I);
			Function *f = call->getCalledFunction();
			if(!f) 
				errs() << "Find a CallInst: "<< *I <<"\n" << "But can't find which function it calls.\n";        
		// **************************Deal with Function isDefined****************************
			if(!f->isDeclaration()) {
				cfg->stateList.push_back(*s);

				string callFunc = f->getName();
				map<string ,int >::iterator it=cfg->funcTime.find(callFunc);
			    	int t = 0;
			    	if(it==cfg->funcTime.end())
					cfg->funcTime.insert(pair<string,int>(callFunc,t));
			    	else
					t = ++it->second;

				readFunc(callFunc, cfg, t);
				id = counter_state++;
				string  str = convertToString(id);
				name = "s"+str;
            			s = new State(false, id, name);
		        	global_CFG.InsertCFGState(id,name);

				string funcName = callFunc;
				if(t>0)
					funcName = callFunc+convertToString(t);
				W.InsertCFGLabel(cfg, NULL, s, funcName); 
				cfg->retVar.pop_back();
			}
		}
	    }
	    cfg->stateList.push_back(*s);
	}
}

int dfscount = 0;

void ProgramCFG::createSucc(BasicBlock *v){
    (*colors)[v] = GRAY;

    std::vector<CFGNode*> currentNodes ;

    //currentNodes is a node list in which nodes are waiting for find succ.
    currentNodes.push_back(&((*nodes)[v]));//first currentNodes is v

    //errs()<<dfscount++<<" dfs: "<<(*nodes)[v].name<<"\n";

    for(BasicBlock::iterator inst = v->begin(); inst != v->end(); inst ++){
        //errs() << *inst<<"\n";
        if(CallInst* call = dyn_cast<CallInst>(inst)){

            if(isa<UnreachableInst>(++inst)){//exit ,abort ,xalloc_die, program exit.
                //	errs()<< *(--inst)<<"\n"; 
                goto finish;
            }
            --inst; 
            Function *f = call->getCalledFunction();

            //If there is a call asm instruction or function pointer call,
            // the return value of getCalledFunction will be null.
            //So we just ignore it.

            if(!f){ 
//                errs() << "Find a CallInst: "<< *inst <<"\n" << "But can't find which function it calls.\n";
                continue;
            }
            if(f->isDeclaration()) {
//                		errs()<<"isDeclaration " << f->getName() << "\n";
                continue;
            }else{
//               errs() << "hasDefinition " << f->getName() << "\n";
               }
            //only concerns the function in the targetFunctionList
            //	if(targetFunctionList->find(f) == targetFunctionList->end()) continue; 

            //	errs() << "find a call : "<< f->getName() << "\n "; 

            BasicBlock *entry = &f->getEntryBlock(); 
            CFGNode *entryNode =  &((*nodes)[entry]);//f's EntryBlock 
            while(!currentNodes.empty()){
                //link succ and prev to each other
                currentNodes.back()->addSucc(entryNode);
                entryNode->addPrev(currentNodes.back());
                currentNodes.pop_back();
            }

            if((*colors)[entry] == WHITE){//dfs
                createSucc(entry);
            }

            for(std::vector<BasicBlock*>::iterator ret= (*retBlocks)[f].begin();
                    ret != (*retBlocks)[f].end(); 
                    ret++){
                currentNodes.push_back(&((*nodes)[*ret]));
            }
        }
    }
    //assert(currentNodes.size()==1);
    while(!currentNodes.empty()){
        CFGNode* current = currentNodes.back();
        currentNodes.pop_back();
        for(succ_iterator succ = succ_begin(v),end = succ_end(v);
                succ != end;
                succ++){
            CFGNode *succNode = &((*nodes)[*succ]);
            current->addSucc(succNode);
            succNode->addPrev(current);
            if((*colors)[*succ] == WHITE){
                createSucc(*succ);
            }
        }
    }
finish:

    //errs()<<"dfs back\n";

    (*colors)[v] = BLACK;
}


//test programCFG,CHECK THE program cfg 
void ProgramCFG::bfs(CFGNode *r){
    if (r == NULL) return; 
    deque<CFGNode*> q;
    (*colors)[r->bb] = GRAY;
    q.push_front(r);
    errs() <<"WHITE:"<<WHITE<<"BLACK:"<<BLACK<<"GRAY:"<<GRAY<<"\n";
    while(!q.empty()){
        r = q.back();
        q.pop_back();
        errs() << r->name<<"\n  succ: {";
        Succ *child = r->getFirstSucc();
        while(child){
            errs()<< child->v->name<<"("<<(*colors)[child->v->bb]<<"), ";
            if((*colors)[child->v->bb] == WHITE){//not visit
                //errs() << "add: "<<child->v->name;
                (*colors)[child->v->bb] = GRAY;
                q.push_front(child->v);
            }
            child = child->nextSucc;
        }
        (*colors)[r->bb] = BLACK;
        errs() << "}\n  prev: { ";
        Prev *prev = r->getFirstPrev();
        while(prev){
            errs() << prev -> v->name << "(" << (*colors)[prev->v->bb] << "), ";
            prev = prev->nextPrev;
        }
        errs() << "}\n";

    }
}

