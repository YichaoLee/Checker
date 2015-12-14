#include "CFG.h"
using namespace std;
int Transition::tran_id = 0;

void CFG::InsertCFGState(int id,string name)
{
	State* tmp = new State(id,name);
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
}

//stateList&transitionList----->Map
bool CFG::initial(){
	stateMap.clear();
	stateStrMap.clear();
	nameMap.clear();
	transitionMap.clear();
	transitionStrMap.clear();
	this->counter_transition = 0;	

	int count=1;
	for(unsigned int i=0;i<stateList.size();i++){	
		State* st=&stateList[i];
		st->transList.clear();
		if(i==0){
			st->ID=0;
		}
		else
			st->ID=count++;		
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
			errs()<<"error: can not find the tostate of the transition: "<<tran->name<<"\n";
			errs()<<tran->name<<" toLabel "<<tran->toLabel<<"\n";
			return false;
		}
	}
	if(initialState==NULL){
		errs()<<"error: there is no initial state\n";
		State* st=&stateList[0];
		st->isInitial=true;
		initialState=st;
	}
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
		case PTR:errs()<<" getptr ";break;
		case NONE:errs()<<" ";break;
	}
	return os;
}



extern string get_m_Operator_str(Op_m op){
	switch(op){
		case PTR:return " getptr ";break;
		case ADD:return " + ";break;
		case SUB:return " - ";break;
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
		case NONE:return " ";break;
	}
}

/******************************dReal_nonlinear_constraint************************************/
raw_ostream& operator << (raw_ostream& os, Variable& object){
	errs()<<"name:"<<object.name<<"id:"<<object.ID<<"\n";
	return os;
}

raw_ostream&  operator << (raw_ostream& os,ParaVariable& object){
	if(object.lvar!=NULL)
	errs()<<object.lvar->name<<object.op<<object.rvar->name;
	else 
	errs()<<object.op<<object.rvar->name;
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
	errs()<<"Location Name:"<<object.name<<" ID:"<<object.ID<<" nextS:"<<object.nextS<<"\n";
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
        errs() << ", " << variableList[i].ID << "\n";
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
    else if(str=="sdiv"||str=="fdiv")
    	return DIV;
    else if(str=="add"||str=="fadd")
    	return ADD;
    else if(str=="sub"||str=="fsub")
    	return SUB;
    else if(str=="log10")
	return LOG10;
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
