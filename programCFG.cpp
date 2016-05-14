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


//Command line argument
/*
cl::opt<int>
lineNo("line",
        cl::desc("Line Number"), cl::value_desc("line"));
*/
cl::opt<string>
funcname("func",
        cl::desc("Function Name"), cl::value_desc("function name"));
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

string& trim(string &str, string::size_type pos = 0)
{
    static const string delim = " \t"; //删除空格或者tab字符
    pos = str.find_first_of(delim, pos);
    if (pos == string::npos)
        return str;
    return trim(str.erase(pos, 1));
}


ParaVariable StringToPara(string checkstr, string funcName){
	ParaVariable ptemp;
	return ptemp;
}

Constraint StringToConstraints(string checkstr, string funcName){

    Constraint ctemp;
/*    ParaVariable ptemp;

//      ctemp.lpvList.clear();
//      ctemp.rpvList.clear();
        int opl=0;
        int ltemp=checkstr.length();
        char c[ltemp];
        for(int m=0;m<ltemp;m++){
            c[m]=checkstr.at(m);
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
            ctemp.op=ASSIGN;
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
*/	
	return ctemp;
}

CFG *Add_target(CFG *cfg,int target,string checkstr){
    check = trim(checkstr);
    CFG *cfg1=new CFG();
    cfg1=cfg;
    int len=check.length();
    char x[len];
    int numCheck=1;//count of equations
    for(int i=0;i<len;i++){
        x[i]=check.at(i);
        if(x[i]=='&')
            numCheck++;
    }
    State *old_target=cfg->searchState(target);
    string funcName = old_target->funcName;
    State *new_target=new State(false,-1,"q1",funcName);
    new_target->transList.clear();
    new_target->consList.clear();
    Transition *temp=new Transition(-2,"p1");
    temp->fromState=old_target;
    temp->fromName=cfg->getNodeName(target);
    temp->level=temp->fromState->level+1;
    temp->toState=new_target;
    new_target->level = temp->level;
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

    Constraint cTemp;
    for(int i=0;i<numCheck;i++){
	cTemp = StringToConstraints(b[i], funcName);
	temp->guardList.push_back(cTemp);
    }

    cfg1->stateList.push_back(*new_target);
    cfg1->transitionList.push_back(*temp);
    return cfg1;
}

ProgramCFG::ProgramCFG(Module &m):M(m){

    root = NULL;
    nodes = NULL;

    clock_t start,finish;

    start=clock();
    errs() <<"START CHECK FUNCTION <"<<funcname<<">~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    //Create the cfg Structure
    CFG* cfg = new CFG();
    cfg->counter_state = 0;
    cfg->counter_variable = 0;
    cfg->counter_s_state = 0;
    cfg->counter_state = 0;
    buildProgramCFG(m, cfg);
    CFG *cfg1=new CFG();
    cfg->initial();

    finish=clock();
    double buildTime = 1000*(double)(finish-start)/CLOCKS_PER_SEC;
    errs() << "BUILDCFG Time: \t" << convertToString(buildTime) << "ms\n";
	
    int inputbound=bound;
    double dreal_time=0;

    if(check=="")
    {
        clock_t start,finish;
        start=clock();
        BoundedVerification verify(cfg,inputbound,target);   
        verify.check(dreal_time,check);
        finish=clock();
    }
    else {
        cfg1=Add_target(cfg,target[0],check);
        cfg1->initial();
        vector<int> new_target;
        for(unsigned int i=0;i<cfg1->stateList.size();i++){
            if(cfg1->stateList[i].name.at(0)=='q')
                new_target.push_back(cfg1->stateList[i].ID);
        }
        clock_t start,finish;
        start=clock();
        BoundedVerification verify(cfg1,inputbound,new_target);
        verify.check(dreal_time,check);
        finish=clock();
    
    }
    errs() << "bound:\t" << bound <<"\tfunctionName:\t" << funcname << "\tcheck:\t" << check << "\n";
    errs() << "Time: \t" << convertToString(1000*(double)(finish-start)/CLOCKS_PER_SEC) << "s \n";

    errs() << "BUILDCFG Time: \t" << convertToString(buildTime) << "ms\n";
	errs() << "Dreal Time: \t" << convertToString(dreal_time/1000) << "s\n";
	double mem_used_peak = memUsedPeak();
	double cpu_time = cpuTime();
	char mem_str[64];
	if (mem_used_peak != 0) 
		sprintf(mem_str, "Memory used: %.2f MB\n", mem_used_peak);
	char time_str[64];
	sprintf(time_str, "CPU Time: %g s\n", cpu_time);
	errs()<<mem_str<<time_str;
}

enum color{WHITE,BLACK,GRAY};
std::map<BasicBlock*, color> *colors;

std::map<Function*, std::vector<BasicBlock*> > *retBlocks;
std::set<Function*> *targetFunctionList; //only use in search


void ProgramCFG::findAllRetBlocks(Module &m){
    for(Module::iterator f = m.begin(); f!= m.end(); f++){
        if(f->getName() == "main") continue;
        for(Function::iterator bb = f->begin(); bb != f->end(); bb++){
            TerminatorInst *terminator = bb->getTerminator();
            if(isa<ReturnInst>(terminator) ){
                ((*retBlocks)[f]).push_back(bb);
            }
        }
    }
}

void  ProgramCFG::setFuncVariable(const Function *F,string func, CFG* cfg, bool initial){
	for (Function::const_arg_iterator it = F->arg_begin(), E = F->arg_end();it != E; ++it) {
		Type *Ty = it->getType();
		if(initial){
			string varNum = it->getName();
			string varName = func+"_"+varNum;
			
			if(Ty->isPointerTy()){
				Type *ETy = Ty->getPointerElementType();
				int ID = cfg->counter_variable++;
				Variable var(varName, ID, PTR);
				cfg->variableList.push_back(var);
				
				InstParser::setVariable(cfg, NULL, ETy, varName, true);
				
			}
			else{
                VarType type;
                if(Ty->isIntegerTy())
                    type = INT;
                else if(Ty->isFloatingPointTy())
                    type = FP;
                else
                    errs()<<"0:programCFG.type error\n";
				int ID = cfg->counter_variable++;
				Variable var(varName, ID, type);
				cfg->variableList.push_back(var);
				cfg->mainInput.push_back(ID);
			}
		}
		else{
			int ID = cfg->counter_variable++;
			string varNum = it->getName();
			string varName = func+"_"+varNum;
			
            VarType type;
			if(Ty->isPointerTy())
				type = PTR;
			else if(Ty->isIntegerTy())
                type = INT;
            else if(Ty->isFloatingPointTy())
                type = FP;
            else
                errs()<<"1:programCFG.type error\n";

			if(!cfg->hasVariable(varName)){
				Variable var(varName, ID, type);
				cfg->variableList.push_back(var);
			}
			else
				errs()<<"1:setFuncVariable error 10086!!\t"<<varName<<"\n";
		}
	}
}

//build program cfg in the main source file !
void  ProgramCFG::buildProgramCFG(Module &m, CFG* cfg){
		if(funcname == "main"){
			cfg->startFunc = "main";
			readFunc("main", cfg, 0);
		}
		else{
			errs() <<  "Warning: There is no main function in the Module!\n";
    			const Function *F = m.getFunction(funcname);
			if(!F)
				errs() <<  "Error: Can't find function "<<funcname<<" in the Module!\n";
			setFuncVariable(F, funcname, cfg, true);
			cfg->startFunc = funcname;

            map<string ,int >::iterator it=cfg->funcTime.find(funcname);
            if(it==cfg->funcTime.end())
                cfg->funcTime.insert(pair<string,int>(funcname,0));
             else
                errs()<<"ProgramCFG::buildProgramCFG error "<<funcname<<"\n";

			readFunc(funcname, cfg, 0);
		}	
}

void ProgramCFG::readFunc(string funcName, CFG *cfg, int time){
	Function *F = M.getFunction(funcName);  
    	if(F == NULL){
    	    	errs() <<  "error readFunc 10086!\t"<<funcName<<"\n";
		exit(-1) ;
	}

    for(Function::iterator bb = F->begin(); bb != F->end(); bb++){
        readBasicblock(bb, cfg, time);
    }
}

void ProgramCFG::readBasicblock(BasicBlock *b, CFG *cfg, int time){
	string callFunc;

	if(b){
	    const Function* F = b->getParent()?b->getParent():nullptr;
	    string func = F->getName();
	    if(time>0)
		func = func+convertToString(time);
            //Generate a new state
        int id = cfg->counter_state++;
        string  str = convertToString(cfg->counter_s_state++);
        string name = "s"+str;
        State* s = new State(false, id, name, func);
		while(!cfg->initialCons.empty()){
			Constraint cTemp = cfg->initialCons.front();
			cfg->initialCons.pop_front();
			s->consList.push_back(cTemp);
		}
        cfg->stateList.resize(id+1);
        raw_ostream &ROS = errs();
        formatted_raw_ostream OS(ROS);

        global_CFG.InsertCFGState(cfg->counter_state,name,func);
            
//	    errs()<<"0:readBasicblock "<<func<<"\n";
	    if(func==cfg->startFunc && b==F->begin()){
            s->level=0;
			cfg->initialState=s;
			s->isInitial=true;
        }
	    else if(b==F->begin()){
			setFuncVariable(F, func, cfg);
	    }

            //Generate the new vector<Constraint>
        BasicBlock::iterator it_end = b->end();
        for(BasicBlock::iterator it = b->begin(); it != it_end; it ++){              
            const Instruction* I = dyn_cast<Instruction>(it);
			SlotTracker SlotTable(F);
            const Module* M = F?F->getParent():nullptr;
            InstParser W(OS, SlotTable, M, nullptr);
//		    errs()<<"0.InL:";W.printInstructionLine(*I);
                //create the LabelMap
            if(it == b->begin()){
                bool hasFromS = W.InsertCFGLabel(cfg,b,s, func, "", false);
                if(!hasFromS){
                    cfg->counter_state--;
                    cfg->counter_s_state--;
                    return;
                }
                else if(s->level>bound){
                    if(cfg->stateList.size()<id)
                        cfg->stateList.resize(id);
                    cfg->stateList[s->ID]=(*s);
                    return;
                }
			} 

            // create the constraint table(NOT TRANSITION)
            W.setConstraint(cfg, s, it, func, target, bound);
				
			string op = I->getOpcodeName();
//			errs()<<"1:readBasicblock "<<func<<":"<<op<<"\n";
			if(op=="call"){
				const CallInst *call = dyn_cast<CallInst>(I);
				Function *f = call->getCalledFunction();
				if(!f) 
					errs() << "Find a CallInst: "<< *I <<"\n" << "But can't find which function it calls.\n";        
			// **************************Deal with Function isDefined****************************
				if(!f->isDeclaration()) {
					cfg->stateList[s->ID]=(*s);

					string callFunc = f->getName();
					map<string ,int >::iterator it=cfg->funcTime.find(callFunc);
					int t = 0;
					if(it==cfg->funcTime.end())
						cfg->funcTime.insert(pair<string,int>(callFunc,t));
					else
						t = ++it->second;

					readFunc(callFunc, cfg, t);
					id = cfg->counter_state++;
					string  str = convertToString(cfg->counter_s_state++);
					name = "s"+str;
					global_CFG.InsertCFGState(id,name,func);

					string funcName = callFunc;
					if(t>0)
						funcName = callFunc+convertToString(t);
					s = new State(false, id, name, func);
					cfg->stateList.resize(id+1);
					bool hasFromS = W.InsertCFGLabel(cfg, b, s, func, funcName+"_ret",true);
                    cfg->retVar.pop_back();
                    if(!hasFromS){
                        cfg->counter_state--;
                        cfg->counter_s_state--;
                        return;
                    }
                    else if(s->level>bound){
                        if(cfg->stateList.size()<id)
                            cfg->stateList.resize(id);
                        cfg->stateList[s->ID]=(*s);
                        return;
                    }
				}
			}
	    }
	    cfg->stateList[s->ID] = (*s);
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

