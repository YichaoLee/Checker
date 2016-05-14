//Define the CFG's Data Structure
#ifndef  _CFG_H___
#define  _CFG_H___
#include <iostream>
#include <string>
#include <vector> 
#include <list>
#include <fstream>
#include <map>
#include <stdlib.h>
#include <assert.h>
#include "llvm/IR/BasicBlock.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h" 
#include <vector> 
#include "convinent.h"


using namespace std;
using namespace llvm;

enum Operator{
    LT,LE,GT,GE,EQ,NE,ASSIGN
};

enum Op_m{
    AND,NAND,OR,XOR,ADD,SUB,TAN,ATAN,ATAN2,SIN,ASIN,COS,ACOS,SQRT,POW,LOG,ABS,EXP,
    SINH,COSH,TANH,MUL,DIV,NONE,LOG10,ADDR,GETPTR,STORE,LOAD,ALLOCA,SREM,ASHR,SHL,
    lt,le,gt,ge,eq,ne
};

extern Operator getEnumOperator(string str);
extern Op_m get_m_Operator(string str);
extern string get_m_Operator_str(Op_m op);
extern raw_ostream& operator << (raw_ostream& os,Op_m& object);
string intToString(int value);

/*************************dReal_nonlinear*************************/
enum VarType{
	NUM,	//The var is a num
	INT,	//The var store a INT type data
    FP,     //The var store a FP type data
	PTR,	//The var store a ID of a PTR var
    BOOL    //The var store a bool type data
};

class Variable{
    public:
        VarType type;
        string name;
        int ID;
        Variable(){type=FP;}
        Variable(string name1,int id,VarType ty):name(name1),ID(id),type(ty){};
		Variable(const Variable &a){
			this->name=a.name;
			this->ID=a.ID;
			this->type=a.type;
		}
		Variable(Variable *a){
			this->name=a->name;
			this->ID=a->ID;
			this->type=a->type;
		}
        void print(){errs()<<"name="<<name<<";id="<<ID;}
        Variable& operator =(const Variable &a){
            this->name=a.name;
            this->ID=a.ID;
	    this->type=a.type;
            return *this;
        }
        friend raw_ostream& operator << (raw_ostream& os, Variable& object);
};

class ParaVariable{
    public:
	bool isExp;
	Op_m op;
        Variable* lvar;
	Variable* rvar;
	string parameter;
        ParaVariable(){isExp=false;lvar=NULL;rvar=NULL,op=NONE;}
        ParaVariable(bool iE,Variable *lv,Variable *rv,string pm,Op_m opr ):isExp(iE),lvar(lv),rvar(rv),parameter(pm),op(opr){};
	ParaVariable(const ParaVariable &a){
	    this->isExp=a.isExp;
            this->lvar=a.lvar;
            this->rvar=a.rvar;
	    this->op=a.op;
            this->parameter=a.parameter;
	}
	~ParaVariable(){isExp=false;lvar=NULL;rvar=NULL;op=NONE;}
        void reverse_parameter();
        ParaVariable& operator =(const ParaVariable &a){
	    this->isExp=a.isExp;
            this->lvar=a.lvar;
            this->rvar=a.rvar;
	    this->op=a.op;
            this->parameter=a.parameter;
            return *this;
        }
        void print(){
	    if(lvar!=NULL)
            	lvar->print();
            errs()<<op;
	    rvar->print();
	    errs()<<"\n";
        }
        friend raw_ostream& operator << (raw_ostream& os,ParaVariable& object);
};


class Constraint{
    public:
        ParaVariable lpvList;
        ParaVariable rpvList;
        Operator op;
    	Constraint(){};
    	~Constraint(){};
        Constraint& operator =(const Constraint &a){
            this->lpvList=a.lpvList;
            this->rpvList=a.rpvList;
            this->op=a.op;
            return *this;
        }
        friend raw_ostream& operator<<(raw_ostream& os, Constraint& object);
};


/*
struct CodeLoc{
    int Line;
    String Loc;
};
*/
class Transition;
class State{
    public:
        bool isInitial;
        int ID;
    	string funcName;
    	int nextS;
        string name;
        int level;
        vector<Transition *> transList;     //zhuanyi List
        vector<Constraint> consList;        //fuzhi   List
        string ContentRec;                  //
        vector<int> locList;                //line number of the code
    	State(){
            this->level = -1;
            this->ID = 0;
            this->name = "";
            this->funcName = "";
            this->isInitial = false;
    	    nextS = -1;
    	}
        State(int id,string name,string funcName)
        {
            this->level = -1;
            this->ID = id;
            this->name = name;
			this->funcName = funcName;
            this->isInitial = false;
			nextS = -1;
        }
        State(bool bo,int id,string name1,string funcName1):isInitial(bo),ID(id),name(name1),funcName(funcName1),nextS(-1){level = -1;};
        void InsertTransition(Transition* tr){ this->transList.push_back(tr);};
        void InsertConstraint(Constraint tr){ this->consList.push_back(tr);};
        State& operator =(const State &a){
            this->isInitial=a.isInitial;
            this->ID=a.ID;
            this->name=a.name;
			this->funcName = a.funcName;
			this->nextS=a.nextS;
            this->transList=a.transList;
            this->consList=a.consList;
            this->locList=a.locList;
            this->level=a.level;
            return *this;
        }
        friend raw_ostream& operator << (raw_ostream& os, State object);
};

class Transition{
    public:
        int ID;
        static int tran_id;
        string name;
        State* fromState;
        State* toState;
        string fromName;
        string toName;
        string toLabel;
        int level;
        vector<Constraint> guardList;
        Transition(int id,string name1):ID(id),name(name1){toLabel="";level =-1;};
        Transition(string fromName,string toName)
        {
            this->fromName = fromName;
            this->toName = toName;
            this->ID = tran_id++;
            this->level = -1;
        }
        Transition& operator =(const Transition a){
            this->ID=a.ID;
            this->tran_id=a.tran_id;
            this->name=a.name;
            this->fromName=a.fromName;
            this->toName=a.toName;
            this->fromState=a.fromState;
            this->toState=a.toState;
            this->toLabel=a.toLabel;
            this->guardList=a.guardList;
            this->level=a.level;
            return *this;
        }
        friend raw_ostream& operator << (raw_ostream& os, Transition object);
};	

class CFG{
    private:
        map<int,State*> stateMap;
        map<int,Transition*> transitionMap;
        map<string,State*> stateStrMap;
        map<string,Transition*> transitionStrMap;
        map<int,string> nameMap;
    public:
        map<string,State*> LabelMap;
		map<string,vector<string>> CallLabel;
		map<string, int> funcTime;
        map<string, string> endBlock;
        vector<string> retVar;
		list<ParaVariable> callVar;
		list<Constraint> initialCons;
        
        string name;
		string startFunc;
		int counter_state;
		int counter_s_state;
		int counter_q_state;
		int counter_variable;
		int counter_data;
        State* initialState;
        vector<State> stateList;
        vector<Transition> transitionList;//at the same time  ,equals to the transList
        vector<Variable> variableList;
        vector<int> mainInput;
        //vector<Transition*> transitionList1;
        Constraint c_tmp1;
        Constraint c_tmp2;
        //int transitionNum;
        void print();
        bool initial();
        bool is_state(const int ID);
        bool hasVariable(string name);
        Variable* getVariable(string name);
        void InsertCFGState(int id,string name,string funcName);
        void InsertCFGTransition(Transition* tr);
        void InsertCFGLabel(string Label, State *s);
        void CFGStateConsList(int id,int op1);
        State* getState(int id){ return stateMap.find(id)->second;};
        State* getLabelState(string Label){
            map<string ,State* >::iterator l_it;
            l_it=LabelMap.find(Label);
            if(l_it==LabelMap.end())
                return NULL;
            else
                return l_it->second;};
		string getNodeName(int i);
        State* searchState(int stateID);
        State* searchState(string name);
        Transition* searchTransition(string name);
        Transition* searchTransition(int transID);
        Transition* searchTransitionByState(int from,int to);
        int counter_transition;//static int counter_transition;
       	CFG& operator =(const CFG a){
            this->name=a.name;
            this->initialState=a.initialState;
            this->stateList=a.stateList;
            this->transitionList=a.transitionList;
            this->variableList=a.variableList;
            return *this;
        };

};
extern CFG global_CFG;
#endif
