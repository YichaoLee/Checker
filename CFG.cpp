#include "CFG.h"
using namespace std;
int Transition::tran_id = 0;

void CFG::InsertCFGState(int id,string name,string funcName)
{
	State* tmp = new State(id,name,funcName);
	this->stateMap.insert(pair<int,State*>(tmp->ID,tmp));
}

CFG  global_CFG;

 void CFG::InsertCFGTransition(Transition* tr)
 {
 	this->transitionMap.insert(pair<int,Transition*>(tr->ID,tr));
 }
void ParaVariable::reverse_parameter(){
	const char* p=parameter.c_str();
	if(p[0]=='-')
	  parameter=string(p+1);
	else
	  parameter="-"+parameter;
}

void CFG::InsertCFGLabel(string Label, State *s){
	LabelMap.insert( pair<string,State*> (Label,s));

        for(unsigned int i=0;i<transitionList.size();i++){

                if(transitionList[i].toLabel == Label&&transitionList[i].toState==NULL)
                {                   
                    transitionList[i].toName = s->name;
                    transitionList[i].toState = s;
                    //errs()<<"SLOT%"<<Label<<"\tsName:\t"<<s->name<<"\n\n\n";
                };
        }   
        for(unsigned int i=0;i<stateList.size();i++)
        {
            State s1= stateList[i];
            for(unsigned int j=0;j<s1.transList.size();j++)
            {
                if(s1.transList[j]->toLabel == Label && s1.transList[j]->toState == NULL)
                {
                    s1.transList[j]->toName = s->name;
                    s1.transList[j]->toState = s;
                }
            }
        }
        errs()<<"func\t"<<Label <<"\t"<< s->name<<"\n";
}

//stateList&transitionList----->Map
bool CFG::initial(){
	stateMap.clear();
	stateStrMap.clear();
	nameMap.clear();
	transitionMap.clear();
	transitionStrMap.clear();
	this->counter_transition = 0;	
/*
	    for(int i=0;i<stateList.size();i++)
		errs()<<stateList[i]<<"\n";
	
	    for(int i=0;i<transitionList.size();i++)
		errs()<<transitionList[i]<<"\n";
*/
	int count=1;
	for(unsigned int i=0;i<stateList.size();i++){	
		State* st=&stateList[i];
//		errs()<<st->ID<<"\t"<<st->nextS<<"\n";
		st->transList.clear();
		if(i==0){
//			initialState=st;
			st->ID=0;
//			st->isInitial=true;
		}
		else
			st->ID=count++;	
		if(st->isInitial)
			initialState=st;
		stateMap.insert(pair<int, State*>(st->ID, st));
		stateStrMap.insert(pair<string, State*>(st->name, st));
		nameMap.insert(pair<int, string>(st->ID, st->name));
		for(unsigned int j=0;j<transitionList.size();j++){
			Transition* tran=&transitionList[j];
			if(st->name==tran->fromName){
				st->transList.push_back(tran);
				tran->fromState=st;
			}
			else if(st->name==tran->toName){				
				tran->toState=st;
			}
		}
	}
	for(unsigned int i=0;i<transitionList.size();i++){
		Transition* tran=&transitionList[i];
		tran->ID=count++;		
		transitionMap.insert(pair<int, Transition*>(tran->ID,tran));
		transitionStrMap.insert(pair<string, Transition*>(tran->name, tran));
		nameMap.insert(pair<int, string>(tran->ID, tran->name));
		if(tran->toState==NULL){
			errs()<<"warning: can not find the tostate of the transition: "<<tran->name<<"\n";
			errs()<<tran->name<<" toLabel "<<tran->toLabel<<"\n";
//			return false;
		}
	}
	if(initialState==NULL){
		errs()<<"Warning: there is no initial state.\n";
		State* st=&stateList[0];
		st->isInitial=true;
		initialState=st;
//		return false;
	}
//	errs()<<"cfg initialed~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
	return true;	

}
bool CFG::is_state(const int ID){
	if((unsigned)ID < stateList.size())
	  return true;
	return false;
}
bool CFG::hasVariable(string name){
    for(unsigned int i = 0; i < variableList.size(); i ++){
        if(variableList[i].name == name)
            return true;
    }
    return false;
}
Variable* CFG::getVariable(string name){
    Variable* var = NULL;
    for(unsigned int i = 0; i < variableList.size(); i ++){
        if(variableList[i].name == name)
            var = &(variableList[i]);
    }
    return var;
}
string CFG::getNodeName(int i){
	if(nameMap.find(i)==nameMap.end())
		return string();
	return nameMap[i];
}
State* CFG::searchState(int stateID) {
	if(stateMap.find(stateID)==stateMap.end())
		return NULL;
	return stateMap[stateID];
}

Transition* CFG::searchTransition(int transID) {		
	if(transitionMap.find(transID)==transitionMap.end())
		return NULL;
	return transitionMap[transID];
}


raw_ostream& operator << (raw_ostream& os,Op_m& object){
	switch(object){
		case ADD:errs()<<" + ";break;
		case SUB:errs()<<" - ";break;
		case AND:errs()<<" and ";break;
		case NAND:errs()<<" nand ";break;
		case OR:errs()<<" or ";break;
		case XOR:errs()<<" xor ";break;
		case TAN:errs()<<" tan ";break;
		case ATAN:errs()<<" atan ";break;
		case ATAN2:errs()<<" atan2 ";break;
		case SIN:errs()<<" sin ";break;
		case ASIN:errs()<<" asin ";break;
		case COS:errs()<<" cos ";break;
		case ACOS:errs()<<" acos ";break;
		case SQRT:errs()<<" sqrt ";break;
		case POW:errs()<<" pow ";break;
		case LOG:errs()<<" log ";break;
		case LOG10:errs()<<" log10 ";break;
		case ABS:errs()<<" abs ";break;
		case EXP:errs()<<" exp ";break;
		case SINH:errs()<<" sinh ";break;
		case COSH:errs()<<" cosh ";break;
		case TANH:errs()<<" tanh ";break;
		case MUL:errs()<<" * ";break;
		case DIV:errs()<<" / ";break;
		case GETPTR:errs()<<" getptr ";break;
		case ADDR:errs()<<" addr ";break;
		case STORE:errs()<<" store ";break;
		case LOAD:errs()<<" load ";break;
		case ALLOCA:errs()<<" alloca ";break;
		case lt:errs()<<" < ";break;
		case le:errs()<<" <= ";break;
		case gt:errs()<<" > ";break;
		case ge:errs()<<" >= ";break;
		case eq:errs()<<" == ";break;
		case ne:errs()<<" != ";break;
		case SREM:errs()<<" % ";break;
		case ASHR:errs()<<" >> ";break;
		case SHL:errs()<<" << ";break;
		case NONE:errs()<<" ";break;
	}
	return os;
}



extern string get_m_Operator_str(Op_m op){
	switch(op){
		case GETPTR:return " getptr ";break;
		case ADD:return " + ";break;
		case SUB:return " - ";break;
		case AND:return " and ";break;
		case NAND:return " nand ";break;
		case OR:return " or ";break;
		case XOR:return " xor ";break;
		case TAN:return " tan ";break;
		case ATAN:return " atan ";break;
		case ATAN2:return " atan2 ";break;
		case SIN:return " sin ";break;
		case ASIN:return " asin ";break;
		case COS:return " cos ";break;
		case ACOS:return " acos ";break;
		case SQRT:return " sqrt ";break;
		case POW:return " pow ";break;
		case LOG:return " log ";break;
		case LOG10:return " log10 ";break;
		case ABS:return " abs ";break;
		case EXP:return " exp ";break;
		case SINH:return " sinh ";break;
		case COSH:return " cosh ";break;
		case TANH:return " tanh ";break;
		case MUL:return " * ";break;
		case DIV:return " / ";break;
		case ADDR:return " addr ";break;
		case STORE:return " store ";break;
		case LOAD:return " load ";break;
		case ALLOCA:return " alloca ";break;
		case lt:return " < ";break;
		case le:return " <= ";break;
		case gt:return " > ";break;
		case ge:return " >= ";break;
		case eq:return " == ";break;
		case ne:return " != ";break;
		case SREM:return " % ";break;
		case ASHR:return " >> ";break;
		case SHL:return " << ";break;
		case NONE:return " ";break;
	}
}

extern string get_var_type(VarType type){
	switch(type){
		case INT: return " INT ";break;
		case FP: return " FP ";break;
		case PTR: return " PTR ";break;
		case BOOL: return " BOOl ";break;
		default: return " ERROR ";break;
	}
}

/******************************dReal_nonlinear_constraint************************************/
raw_ostream& operator << (raw_ostream& os, Variable& object){
	errs()<<"name:"<<object.name<<"id:"<<object.ID<<"\n";
	return os;
}

raw_ostream&  operator << (raw_ostream& os,ParaVariable& object){
	if(object.lvar!=NULL)
		errs()<<object.lvar->name;
	errs()<<object.op;
	if(object.rvar!=NULL)
		errs()<<object.rvar->name;
	return os;
}

raw_ostream& operator << (raw_ostream& os,Constraint& object){
	errs()<<"constraint:";	
	errs()<<object.lpvList;
	switch(object.op){
		case LT:errs()<<" < ";break;
		case LE:errs()<<" <= ";break;
		case GT:errs()<<" > ";break;
		case GE:errs()<<" >= ";break;
		case EQ:errs()<<" == ";break;
		case NE:errs()<<" != ";break;
		case ASSIGN:errs()<<" = ";break;
	}
	errs()<<object.rpvList;

	return os;

}





raw_ostream& operator << (raw_ostream& os, Transition object){
	errs()<<"Transition Name:"<<object.name<<" ID:"<<object.ID<<"\n";
	errs()<<"Level:"<<object.level<<"\n";
	errs()<<"ToLabel:"<<object.toLabel<<"\n";
	errs()<<"from:"<<object.fromName<<" to:"<<object.toName<<"\nGuard:\n";
	if(object.guardList.empty())
		errs()<<"null\n";
	else{
		for(unsigned int i=0;i<object.guardList.size();i++)
			errs()<<object.guardList[i]<<" ";
		errs()<<"\n";
	}
	return os;
}

raw_ostream& operator << (raw_ostream& os, State object){
	errs()<<"Location Name:"<<object.funcName<<" "<<object.name<<" ID:"<<object.ID<<" nextS:"<<object.nextS<<"\n";
	errs()<<"Level:"<<object.level<<"\n";
	if(object.isInitial)
	  errs()<<"\nInitial location\n";
	if(object.consList.empty())
		errs()<<"null\n";
	else{
		for(unsigned int i=0;i<object.consList.size();i++)
			errs()<<object.consList[i]<<" ";
		errs()<<"\n";
	}
	errs()<<object.ContentRec<<"\n";
	return os;
}

void CFG::print(){
	errs()<<"*******************CFG Information*********************\n";
	errs()<<"CFG:"<<name<<"\n";
	for(unsigned int i=0;i<variableList.size();i++){
		errs()<<"variable:"<<variableList[i].name;
        errs() << ", " << variableList[i].ID <<" "<<get_var_type(variableList[i].type)<<"\n";
    }
	for(unsigned int i=0;i<stateList.size();i++)
  		errs()<<stateList[i]<<"\n";
	for(unsigned int i=0;i<transitionList.size();i++)
		errs()<<transitionList[i]<<"\n";
}   
        

Operator getEnumOperator(string str)
{

    if(str=="EQ")
    	return EQ;
    else if(str=="LT")
    	return LT;
    else if(str=="LE")
    	return LE;
    else if(str=="GE")
    	return GE;
    else if(str=="GT")
    	return GT;
    else if(str=="NE")
    	return NE;
    else if(str=="ASSIGN")
    	return ASSIGN;
}

Op_m get_m_Operator(string str){

    if(str=="tan")
    	return TAN;
    else if(str=="atan")
    	return ATAN;
    else if(str=="atan2")
    	return ATAN2;
    else if(str=="sin")
    	return SIN;
    else if(str=="asin")
    	return ASIN;
    else if(str=="cos")
    	return COS;
    else if(str=="acos")
    	return ACOS;
    else if(str=="sqrt")
    	return SQRT;
    else if(str=="pow")
    	return POW;
    else if(str=="log")
    	return LOG;
    else if(str=="abs"||str=="fabs")
    	return ABS;
    else if(str=="exp")
    	return EXP;
    else if(str=="sinh")
    	return SINH;
    else if(str=="cosh")
    	return COSH;
    else if(str=="tanh")
    	return TANH;
    else if(str=="mul"||str=="fmul")
    	return MUL;
    else if(str=="sdiv"||str=="fdiv"||str=="udiv")
    	return DIV;
    else if(str=="add"||str=="fadd")
    	return ADD;
    else if(str=="sub"||str=="fsub")
    	return SUB;
    else if(str=="and")
    	return AND;
    else if(str=="nand")
    	return NAND;
    else if(str=="or")
    	return OR;
    else if(str=="xor")
    	return XOR;
    else if(str=="log10")
		return LOG10;
	else if(str=="EQ")
    	return eq;
    else if(str=="LT")
    	return lt;
    else if(str=="LE")
    	return le;
    else if(str=="GE")
    	return ge;
    else if(str=="GT")
    	return gt;
    else if(str=="NE")
    	return ne;
    else if(str=="srem")
    	return SREM;
    else if(str=="ashr")
    	return ASHR;
    else if(str=="shl")
    	return SHL;
    else
		return NONE;
}


string intToString(int value)  
{  
    char tmpString[15];  
    memset(tmpString, 0x00, 15);  
    sprintf(tmpString, "%d", value);  
    return  tmpString;  
}
