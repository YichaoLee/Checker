#include "Verification.h"
#include "time.h"
using namespace z3;
using namespace std;
int VERBOSE_LEVEL = 0;
int UC_LEVEL=0;
int cont = 0;
//add constraint to empty vector  0==0


 void printVector(vector<int> path){
	for(vector<int>::iterator it=path.begin();it<path.end();it++)
		errs()<<*it<<";";
	errs()<<"\n";
}
 void printIndex(vector<IndexPair> path){
	for(vector<IndexPair>::iterator it=path.begin();it<path.end();it++)
		it->print();
	errs()<<"\n";
}
 void printPara(vector<ParaVariable> path){
	for(vector<ParaVariable>::iterator it=path.begin();it<path.end();it++)
		it->print();
	errs()<<"\n";
}


/***********************************check with dReal*********************************************/

bool Verification::check(CFG* ha, vector<int> path)
{
	clear();

    	printVector(path);

	clock_t start,finish;
    	start=clock();

	encode_path(ha, path);
	
	opensmt_result res = opensmt_check( ctx );

    	finish=clock();
		
	if(res == l_true){
		cerr<<"dreal_result is sat\n";
		return true;
	}
	cerr<<"dreal_result is unsat\n";
	return false;
}

void Verification::smt_push(string str)
{
	smt += "(";
	smt += str;
	smt += ")\n";	
	
}

void Verification::smt_declare(int ID)
{
	smt_push("declare-fun x_"+int2string(ID)+" () Real");
}

void Verification::smt_assert(string str)
{
	smt_push("assert "+str);
}

void Verification::get_constraint(Constraint *con, vector<opensmt_expr> &x, vector<int> &var, bool repeat)
{
	Operator op = con->op;

	ParaVariable pvList=con->rpvList;
	opensmt_expr y; 
	opensmt_expr z;
	if(pvList.lvar!=NULL)
	{
		if(pvList.lvar->isNumber)
			y = opensmt_mk_num_from_string(ctx, pvList.lvar->name.c_str());
		else{	
			int ID = pvList.lvar->ID;
			while(var[ID]>-1)
				ID = var[ID];
			y = x[ID];		
		}
	}	

	if(pvList.rvar->isNumber)
		z = opensmt_mk_num_from_string(ctx, pvList.rvar->name.c_str());
	else{	
		int ID = pvList.rvar->ID;
		while(var[ID]>-1)
			ID = var[ID];
		z = x[ID];		
	}

	opensmt_expr exprr; 
	switch(pvList.op){
		case ADD:
			exprr = opensmt_mk_plus_2(ctx, y, z);
			break;
		case SUB:
			exprr = opensmt_mk_minus(ctx, y, z);
			break;
		case TAN:
			exprr = opensmt_mk_tan(ctx, z);
			break;
		case ATAN:
			exprr = opensmt_mk_atan(ctx, z);
			break;
		case ATAN2:
			exprr = opensmt_mk_atan2(ctx, y, z);
			break;
		case SIN:
			exprr = opensmt_mk_sin(ctx, z);
			break;
		case ASIN:
			exprr = opensmt_mk_asin(ctx, z);
			break;
		case COS:
			exprr = opensmt_mk_cos(ctx, z);
			break;
		case ACOS:
			exprr = opensmt_mk_acos(ctx, z);
			break;
		case SQRT:
			exprr = opensmt_mk_pow(ctx, z, opensmt_mk_num(ctx, 0.5));
			break;
		case POW:
			exprr = opensmt_mk_pow(ctx, y, z);
			break;
		case LOG:
			exprr = opensmt_mk_log(ctx, z);
			break;
		case LOG10:
			exprr = opensmt_mk_div(ctx, opensmt_mk_log(ctx, z),opensmt_mk_log(ctx, opensmt_mk_num(ctx, 10)));
			break;
		case ABS:
			exprr = opensmt_mk_abs(ctx, z);
			break;
		case EXP:
			exprr = opensmt_mk_exp(ctx, z);
			break;
		case SINH:
			exprr = opensmt_mk_sinh(ctx, z);
			break;
		case COSH:
			exprr = opensmt_mk_cosh(ctx, z);
			break;
		case TANH:
			exprr = opensmt_mk_tanh(ctx, z);
			break;
		case MUL:
			exprr = opensmt_mk_times_2(ctx, y, z);
			break;
		case DIV:
			exprr = opensmt_mk_div(ctx, y, z);
			break;
		case NONE:
			exprr = z;
			break;
	}


	pvList=con->lpvList;
	opensmt_expr exprl; 

	if(pvList.rvar->isNumber)
			exprl = opensmt_mk_num_from_string(ctx, pvList.rvar->name.c_str());
	else{
		int ID = pvList.rvar->ID;

		if(repeat == false){
			if(op == ASSIGN)
			{
				int uID = var[ID];
				if(con->rpvList.lvar==NULL){
					if(con->rpvList.rvar->isNumber)
						var[ID]=-2;
					else
						var[ID]=con->rpvList.rvar->ID;
				}
				if(uID != -1)
					return;
			}	
			
		}			
		else{
			if(op == ASSIGN)
			{			
				var[ID] = x.size();
				ID = x.size();
				x.push_back(opensmt_mk_unbounded_real_var(ctx, ("x_"+int2string(ID)).c_str()));
				int v = -1;
				var.push_back(v);
				int uID = var[ID];
				if(con->rpvList.lvar==NULL){
					if(con->rpvList.rvar->isNumber)
						var[ID]=-2;
					else
						var[ID]=con->rpvList.rvar->ID;
				}
				if(uID != -1)
					return;
			}
		}
		while(var[ID]>-1)
			ID = var[ID];

		exprl = x[ID];
	}


	opensmt_expr ast; 
	switch(op){
		case LT:		
			ast = opensmt_mk_lt(ctx, exprl, exprr);
			break;
		case LE:			
			ast = opensmt_mk_leq(ctx, exprl, exprr);
			break;
		case GT:			
			ast = opensmt_mk_gt(ctx, exprl, exprr);
			break;
		case GE:			
			ast = opensmt_mk_geq(ctx, exprl, exprr);
			break;
		case EQ:case ASSIGN:				
			ast = opensmt_mk_eq(ctx, exprl, exprr);
			break;
		case NE:			
			ast = opensmt_mk_not(ctx, opensmt_mk_eq(ctx, exprl, exprr));
			break;
	}
	opensmt_print_expr(ast);
    	opensmt_assert(ctx, ast);
	cerr<< endl;
	
}

string Verification::get_pvList(ParaVariable pvList)
{
	string str = "";
	if(pvList.isExp)
		str += "(";
	str += get_m_Operator_str(pvList.op);
	if(pvList.lvar!=NULL)
		str += (get_Variable(pvList.lvar)+" ");
	str += get_Variable(pvList.rvar);
	if(pvList.isExp)
		str += ")";
	return str;

}



string Verification::get_Variable(Variable *var)
{
	string str = "";
	if(var->isNumber)
		str += var->name;
	else
		str += "x_"+int2string(var->ID);
	return str;
}


void Verification::encode_path(CFG* ha, vector<int> patharray)
{

	int var_num = ha->variableList.size();
	vector<opensmt_expr> x;
	vector<int> var(var_num,-1);
//	opensmt_set_precision(ctx, 1);
	double pre = opensmt_get_precision(ctx);
	cerr<<"Precision is "<<pre<<endl;

	for(unsigned i=0;i<var_num;i++)
	{
		x.push_back(opensmt_mk_unbounded_real_var(ctx, ("x_"+int2string(i)).c_str()));
	}
	
	int state_num =	 (patharray.size()+1)/2;
	int total_state  = ha->stateList.size()+ ha->transitionList.size();
	vector<bool> repeat(total_state,false);
	
	for (int j= 0;j<state_num; j++)
	{	
		State* st=ha->searchState(patharray[2*j]);
		assert(st!=NULL);
		//encode the previous transition guard
		for(unsigned m=0;m<st->consList.size();m++)
		{
			Constraint* con = &st->consList[m];
			get_constraint(con, x, var, repeat[patharray[2*j]]);	
		}
		repeat[patharray[2*j]]=true;
		
		if(j!=state_num-1)	
		{
			Transition* pre=ha->searchTransition(patharray[2*j+1]);
			assert(pre!=NULL);
		//encode the previous transition guard	
	
			for(unsigned m=0;m<pre->guardList.size();m++)
			{
				Constraint* con = &pre->guardList[m];
				get_constraint(con, x, var, repeat[patharray[2*j+1]]);
			}	
			repeat[patharray[2*j+1]]=true;

		}

	}

}


void  BoundedVerification::block_path(int number,CFG *cfg,vector<int> path){
//	vector<IndexPair> indexs = verify.get_core_index();
//printVector(path);
//printIndex(indexs);

		int pathStart = 0;
		int pathEnd   = path.size()/2;
		vector<int> pathsegment;
		for (int i=pathStart;i<pathEnd; i++) {
			pathsegment.push_back(path[2*i]);
			pathsegment.push_back(path[2*i+1]);			
		}
		pathsegment.push_back(path[2*pathEnd]);
		
        errs()<<"IIS Path "<<number<<":"<<get_path_name(cfg,pathsegment)<<"\n\n";	
		
        int loop = bound-(pathEnd-pathStart);
		for(int i=0;i<=loop;i++){
			Minisat::vec<Minisat::Lit> lits;
			for(unsigned j=0;j<pathsegment.size();j++){
				lits.push(~var(i+j/2,pathsegment[j]));	
			//	errs()<<"v("<<i+j/2<<","<<pathsegment[j]<<")"<<"\n";
			}
			s.addClause(lits);			
		}
}


/*******************************class BoundedVerification****************************************/
BoundedVerification::BoundedVerification(CFG* aut, int bound, int target){
	this->cfg=aut;
	this->bound=bound;
	this->target=target;
	num_of_path=0;
}

vector<int> path;
bool result = false;

string get_name(CFG *cfg,vector<int> path){
	string name="";
	for(unsigned i=0;i<path.size();i++){
		if(i%2==0){
		name +=cfg->getNodeName(path[i]);
		if(i != path.size()-1)
		  name += "^";
	}
	}
	return name;
}

void BoundedVerification::DFS(int intbound,int bound,int start,int end){
	int reduntsize=path.size()-2*(intbound-bound);
	if(reduntsize!=0){
		int temp=path.back();
		for(int m=0;m<reduntsize+1;m++)
			path.pop_back();
		path.push_back(temp);
	}

	path.push_back(start);
	if(bound==0||result==true){
		return;
	}
	else if(verify.check(cfg, path)){   //the path is feasible, terminate
		num_of_path++;
		if(start==end){
			reachPath=get_name(cfg,path);
			result = true;
		}
		else {
			for(unsigned int i=0;i<cfg->searchState(start)->transList.size();i++){
				path.push_back(cfg->searchState(start)->transList[i]->ID);
				DFS(intbound,bound-1,cfg->searchState(start)->transList[i]->toState->ID,end);
			}
		}
	}
}

/* Bounded reachability analysis return false:unreachable true:reachable */

bool BoundedVerification::check(int line,string check){
	cfg->print();
//	encode_graph();
//	bool result = solve();
	DFS(bound,bound,cfg->initialState->ID,target);
	ofstream SaveFile("/home/cfg/Documents/test_CFG_v3/benchmarkresult.txt", ios::app);
	string target_name = cfg->getNodeName(target);
	errs()<<"\nNumber of path checked:"<<num_of_path<<"\n";
	if( result ){
		errs()<<"line "<<line<<" is reachable\n";
		errs()<<"Witness:"<<reachPath.c_str()<<"\n";
		SaveFile<<"line "<<line<<" is reachable\n";
		SaveFile<<"Witness:"<<reachPath.c_str()<<"\n";
	}
	else {
		errs()<<"line "<<line<<" is not reachable under bound "<<bound<<" when check="<<check<<"\n";
	}

	SaveFile.close();

	return result;
}

/* return true:reachable false:unreachable  */
bool BoundedVerification::solve(){
	for(int i=0;i<=bound;i++){
		while(true){
			if(s.solve(var(i,target))){
				num_of_path++;
				vector<int> path=decode_path();
				if(verify.check(cfg, path)){   //the path is feasible, terminate
					reachPath=get_path_name(cfg,path);
					return true;
				}
				else{				//infeasible, feed the IIS path to the SAT solver
					//errs()<<"TRY Path: "<<get_path_name(path)<<"\n";
					block_path(num_of_path,cfg,path);
				}
			}
			else
				break;
		}
	}
	return false;
}

/*encode the bounded graph structure of LHA into a propositional formula set*/
void BoundedVerification::encode_graph(){
	Minisat::vec<Minisat::Lit> lits;
	//initial condition	
	for(unsigned i=0;i<cfg->stateList.size();i++){
		State* st = &cfg->stateList[i];
		if(st->isInitial)
			s.addClause(var(0,st->ID));
		else
			s.addClause(~var(0,st->ID));
	}

	//not exactly in one location and transition, exclude condition
	for(int k=0;k<=bound;k++){
		for(unsigned i=0;i<cfg->stateList.size();i++){
			for(unsigned j=i+1;j<cfg->stateList.size();j++){
				s.addClause(~var(k,cfg->stateList[i].ID), ~var(k,cfg->stateList[j].ID));
			}
		}
		for(unsigned i=0;i<cfg->transitionList.size();i++){
			for(unsigned j=i+1;j<cfg->transitionList.size();j++){
				s.addClause(~var(k,cfg->transitionList[i].ID), ~var(k,cfg->transitionList[j].ID));
			}
		}
	}
	// transition relation 
	for(unsigned i=0;i<cfg->stateList.size();i++){
		State* st = &cfg->stateList[i];			
		for(int k=0;k<bound;k++){
			Minisat::Lit x=var(k,st->ID);
			if(st->transList.size()==0){
				s.addClause(~x,var(k+1,st->ID));
				for(unsigned j=0;j<cfg->transitionList.size();j++){
					s.addClause(~x, ~var(k,cfg->transitionList[j].ID));
				}
			}
			else{	
				lits.clear();
				for(unsigned j=0;j<st->transList.size();j++){
					Minisat::Lit next_tran_exp=var(k,st->transList[j]->ID);
					Minisat::Lit next_state_exp=var(k+1,st->transList[j]->toState->ID);
					s.addClause(~x, ~next_tran_exp, next_state_exp);
					lits.push(next_tran_exp);
				}
				lits.push(~x);
				s.addClause(lits);
			}
		}
	}
	//target condition
	lits.clear();
	for(int i=0;i<=bound;i++)
		lits.push(var(i,target));
	s.addClause(lits);
}

/* decode a path from a satisfiable model */
vector<int>  BoundedVerification::decode_path(){
	assert(s.okay());
	int state_num=cfg->stateList.size()+cfg->transitionList.size();;
	int* path=new int[2*bound+1];
	for (int i=1;i<=state_num*(bound+1); i++) {
		if(s.modelValue(i) == Minisat::l_True){
			int id,loop;
			decode(i,loop,id);
			if(cfg->is_state(id))
				path[2*loop]=id;
			else
				path[2*loop+1]=id;
		}
	}
	vector<int> compress_path;
	for(int i=0;i<2*bound+1;i++){
		compress_path.push_back(path[i]);
		if(path[i] == target)
		  break;
	}
		delete[] path;
	return compress_path;
}


string BoundedVerification::get_path_name(CFG *cfg,vector<int> path){
	string name="";
	for(unsigned i=0;i<path.size();i++){
		if(i%2==0){
		name +=cfg->getNodeName(path[i]);
		/*
		name+="(";
		for(vector<int>::iterator it=cfg->searchState(path[i])->locList.begin();it<cfg->searchState(path[i])->locList.end();it++){
        name+=intToString(*it);
        if(it+1!=cfg->searchState(path[i])->locList.end()){
       	name+=",";
       }
    }
    	name+=")";*/
		if(i != path.size()-1)
		  name += "^";
	}
	}
	if(cfg->searchState(path[path.size()-1])->locList.empty()){
		name=name.substr(0,name.length()-5);
	}
	return name;
}
Minisat::Lit BoundedVerification::var(const int loop, const int st){
	int state_num=cfg->stateList.size()+cfg->transitionList.size();;
	int var= state_num*loop+st+1;
	while (var >= s.nVars()-1) s.newVar();
	return Minisat::mkLit(var);
}

void BoundedVerification::decode(int code,int& loop,int& ID){
	code--;
	int state_num=cfg->stateList.size()+cfg->transitionList.size();;
	loop = code/state_num;
	ID = code%state_num;
}
