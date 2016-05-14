#include "Verification.h"
#include "time.h"
using namespace std;
int VERBOSE_LEVEL = 0;
int UC_LEVEL=0;
int cont = 0;
//add constraint to empty vector  0==0

string ConvertToString(double d){
    ostringstream os;
    if(os<<d)
        return os.str();
    return "invalid convertion";
}

double ConvertToDouble(string name){
	stringstream ss;
	double val;
	ss<<name;
	ss>>val;
	return val;
}

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

void printPath(CFG *cfg, vector<int> path){
	cerr<<"Path route: ";
	for(unsigned i=0;i<path.size();i++){
		int id = path[i];
		State *s = cfg->searchState(id);
		if(s==NULL) cerr<<"printPath state error "<<id<<"\n";
		if(i<path.size()-1){
			cerr<<s->name<<"->";
			Transition *t = cfg->searchTransition(path[++i]);
			if(t==NULL) cerr<<"\nprintPath transition error "<<path[i]<<"\n";
			assert(t!=NULL);
			cerr<<t->name<<"->";
		}
		else
			cerr<<s->name<<"("<<s->ID<<")"<<endl;
	}
}

/***********************************check with dReal*********************************************/

bool Verification::check(CFG* ha, vector<int> path)
{
	clear();

	printPath(ha, path);
	
	clock_t start,finish;

	encode_path(ha, path);

    start = clock();
	opensmt_result res = opensmt_check( ctx );

    finish=clock();

	double time_used= 1000*(double)(finish-start)/CLOCKS_PER_SEC;
	time += time_used;
//        errs()<<"Time:\t"<<ConvertToString(time_used)<<"ms\n";
	if(res == l_true){
		cerr<<"dreal_result is sat\n\n\n";
		return true;
	}
	cerr<<"dreal_result is unsat\n\n\n";
	return false;
}

void Verification::print_sol(CFG* cfg, vector<int> x) {
    for(unsigned i=0;i<x.size();i++){

		opensmt_expr mainInput = table->getX(x[i]);

    	double const x_lb = opensmt_get_bound_lb(ctx, mainInput);
    	double const x_ub = opensmt_get_bound_ub(ctx, mainInput);
    	errs()<<cfg->variableList[x[i]].name<<" = ["<<x_lb<<", "<<x_ub<<"]\n";
    }
    return;
}

opensmt_expr Verification::getExpr(Variable *v, bool &treat, double &val, VarTable *table)
{
	opensmt_expr expr=NULL;
	if(v->type==NUM){
		expr = opensmt_mk_num_from_string(ctx, v->name.c_str());
		val = ConvertToDouble(v->name);
	}
	else if(v->type == INT || v->type==FP){
		expr = table->getX(v->ID);
		treat = treat&table->getVal(v->ID, val);
	}
	else
		errs()<<"getExpr error : v->type error\n";
	return expr;
}

opensmt_expr Verification::opensmt_mk_AND(opensmt_context ctx, opensmt_expr y, opensmt_expr z, string name, unsigned num){
	opensmt_expr* xlt = new opensmt_expr[num];
	opensmt_expr* xrt = new opensmt_expr[num];
	opensmt_expr* xt = new opensmt_expr[num];
	vector<opensmt_expr> xl;
	vector<opensmt_expr> xr;

	for(unsigned i=0;i<num;i++){
		string lname = name+"_l"+ConvertToString(i);
		xl.push_back(opensmt_mk_int_var(ctx, lname.c_str(), 0, 1));
		xlt[i] = opensmt_mk_times_2(ctx, xl[i], opensmt_mk_pow(ctx, opensmt_mk_num(ctx, 2), opensmt_mk_num(ctx, i)));
	}
	opensmt_expr ast_l = opensmt_mk_eq(ctx, y, opensmt_mk_plus(ctx, xlt, num));
	opensmt_print_expr(ast_l);
	opensmt_assert(ctx, ast_l);
	cerr<< endl;

	for(unsigned i=0;i<num;i++){
		string rname = name+"_r"+ConvertToString(i);
		xr.push_back(opensmt_mk_int_var(ctx, rname.c_str(), 0, 1));
		xrt[i] = opensmt_mk_times_2(ctx, xr[i], opensmt_mk_pow(ctx, opensmt_mk_num(ctx, 2), opensmt_mk_num(ctx, i)));
	}
	opensmt_expr ast_r = opensmt_mk_eq(ctx, z, opensmt_mk_plus(ctx, xrt, num));
	opensmt_print_expr(ast_r);
	opensmt_assert(ctx, ast_r);
	cerr<< endl;

	for(unsigned i=0; i<num; i++){
		xt[i] = opensmt_mk_times_2(ctx, opensmt_mk_times_2(ctx, xl[i], xr[i]), opensmt_mk_pow(ctx, opensmt_mk_num(ctx, 2), opensmt_mk_num(ctx, i)));
	}

	opensmt_expr ast = opensmt_mk_plus(ctx, xt, num);
	return ast;
}

opensmt_expr Verification::opensmt_mk_NAND(opensmt_context ctx, opensmt_expr y, opensmt_expr z, string name, unsigned num){
	opensmt_expr* xlt = new opensmt_expr[num];
	opensmt_expr* xrt = new opensmt_expr[num];
	opensmt_expr* xt = new opensmt_expr[num];
	vector<opensmt_expr> xl;
	vector<opensmt_expr> xr;

	for(unsigned i=0;i<num;i++){
		string lname = name+"_l"+ConvertToString(i);
		xl.push_back(opensmt_mk_int_var(ctx, lname.c_str(), 0, 1));
		xlt[i] = opensmt_mk_times_2(ctx, xl[i], opensmt_mk_pow(ctx, opensmt_mk_num(ctx, 2), opensmt_mk_num(ctx, i)));
	}
	opensmt_expr ast_l = opensmt_mk_eq(ctx, y, opensmt_mk_plus(ctx, xlt, num));
	opensmt_print_expr(ast_l);
	opensmt_assert(ctx, ast_l);
	cerr<< endl;

	for(unsigned i=0;i<num;i++){
		string rname = name+"_r"+ConvertToString(i);
		xr.push_back(opensmt_mk_int_var(ctx, rname.c_str(), 0, 1));
		xrt[i] = opensmt_mk_times_2(ctx, xr[i], opensmt_mk_pow(ctx, opensmt_mk_num(ctx, 2), opensmt_mk_num(ctx, i)));
	}
	opensmt_expr ast_r = opensmt_mk_eq(ctx, z, opensmt_mk_plus(ctx, xrt, num));
	opensmt_print_expr(ast_r);
	opensmt_assert(ctx, ast_r);
	cerr<< endl;

	for(unsigned i=0; i<num; i++){
		xt[i] = opensmt_mk_times_2(ctx, opensmt_mk_minus(ctx, opensmt_mk_num(ctx, 1), opensmt_mk_times_2(ctx, xl[i], xr[i])), opensmt_mk_pow(ctx, opensmt_mk_num(ctx, 2), opensmt_mk_num(ctx, i)));
	}

	opensmt_expr ast = opensmt_mk_plus(ctx, xt, num);
	return ast;
}

opensmt_expr Verification::opensmt_mk_OR(opensmt_context ctx, opensmt_expr y, opensmt_expr z, string name, unsigned num){
	opensmt_expr* xlt = new opensmt_expr[num];
	opensmt_expr* xrt = new opensmt_expr[num];
	opensmt_expr* xt = new opensmt_expr[num];
	vector<opensmt_expr> xl;
	vector<opensmt_expr> xr;

	for(unsigned i=0;i<num;i++){
		string lname = name+"_l"+ConvertToString(i);
		xl.push_back(opensmt_mk_int_var(ctx, lname.c_str(), 0, 1));
		xlt[i] = opensmt_mk_times_2(ctx, xl[i], opensmt_mk_pow(ctx, opensmt_mk_num(ctx, 2), opensmt_mk_num(ctx, i)));
	}
	opensmt_expr ast_l = opensmt_mk_eq(ctx, y, opensmt_mk_plus(ctx, xlt, num));
	opensmt_print_expr(ast_l);
	opensmt_assert(ctx, ast_l);
	cerr<< endl;

	for(unsigned i=0;i<num;i++){
		string rname = name+"_r"+ConvertToString(i);
		xr.push_back(opensmt_mk_int_var(ctx, rname.c_str(), 0, 1));
		xrt[i] = opensmt_mk_times_2(ctx, xr[i], opensmt_mk_pow(ctx, opensmt_mk_num(ctx, 2), opensmt_mk_num(ctx, i)));
	}
	opensmt_expr ast_r = opensmt_mk_eq(ctx, z, opensmt_mk_plus(ctx, xrt, num));
	opensmt_print_expr(ast_r);
	opensmt_assert(ctx, ast_r);
	cerr<< endl;

	for(unsigned i=0; i<num; i++){
		opensmt_expr xl_t = opensmt_mk_minus(ctx, opensmt_mk_num(ctx, 1), xl[i]);
		opensmt_expr xr_t = opensmt_mk_minus(ctx, opensmt_mk_num(ctx, 1), xr[i]);
		xt[i] = opensmt_mk_times_2(ctx, opensmt_mk_minus(ctx, opensmt_mk_num(ctx, 1), opensmt_mk_times_2(ctx, xl_t, xr_t)), opensmt_mk_pow(ctx, opensmt_mk_num(ctx, 2), opensmt_mk_num(ctx, i)));
	}

	opensmt_expr ast = opensmt_mk_plus(ctx, xt, num);
	return ast;
}

opensmt_expr Verification::opensmt_mk_XOR(opensmt_context ctx, opensmt_expr y, opensmt_expr z, string name, unsigned num){
	opensmt_expr* xlt = new opensmt_expr[num];
	opensmt_expr* xrt = new opensmt_expr[num];
	opensmt_expr* xt = new opensmt_expr[num];
	vector<opensmt_expr> xl;
	vector<opensmt_expr> xr;

	for(unsigned i=0;i<num;i++){
		string lname = name+"_l"+ConvertToString(i);
		xl.push_back(opensmt_mk_int_var(ctx, lname.c_str(), 0, 1));
		xlt[i] = opensmt_mk_times_2(ctx, xl[i], opensmt_mk_pow(ctx, opensmt_mk_num(ctx, 2), opensmt_mk_num(ctx, i)));
	}
	opensmt_expr ast_l = opensmt_mk_eq(ctx, y, opensmt_mk_plus(ctx, xlt, num));
	opensmt_print_expr(ast_l);
	opensmt_assert(ctx, ast_l);
	cerr<< endl;

	for(unsigned i=0;i<num;i++){
		string rname = name+"_r"+ConvertToString(i);
		xr.push_back(opensmt_mk_int_var(ctx, rname.c_str(), 0, 1));
		xrt[i] = opensmt_mk_times_2(ctx, xr[i], opensmt_mk_pow(ctx, opensmt_mk_num(ctx, 2), opensmt_mk_num(ctx, i)));
	}
	opensmt_expr ast_r = opensmt_mk_eq(ctx, z, opensmt_mk_plus(ctx, xrt, num));
	opensmt_print_expr(ast_r);
	opensmt_assert(ctx, ast_r);
	cerr<< endl;

	for(unsigned i=0; i<num; i++){
		opensmt_expr xl_t = opensmt_mk_minus(ctx, opensmt_mk_num(ctx, 1), xl[i]);
		opensmt_expr xr_t = opensmt_mk_minus(ctx, opensmt_mk_num(ctx, 1), xr[i]);
		xt[i] = opensmt_mk_times_2(ctx, opensmt_mk_times_2(ctx, xl_t, xr_t), opensmt_mk_pow(ctx, opensmt_mk_num(ctx, 2), opensmt_mk_num(ctx, i)));
	}

	opensmt_expr ast = opensmt_mk_plus(ctx, xt, num);
	return ast;
}

opensmt_expr Verification::opensmt_mk_SREM(opensmt_context ctx, opensmt_expr y, opensmt_expr z, string name){
	string div_name = name+"_div";
	string real_name = name+"_divreal";
	opensmt_expr div_real = opensmt_mk_unbounded_real_var(ctx, real_name.c_str());
	opensmt_expr div_expr = opensmt_mk_unbounded_int_var(ctx, div_name.c_str());
	opensmt_expr ast_t = opensmt_mk_eq(ctx, div_real, opensmt_mk_div(ctx, y, z));
	opensmt_print_expr(ast_t);
	cerr<<endl;
	opensmt_assert(ctx, ast_t);

	opensmt_expr ast_tleq = opensmt_mk_leq(ctx, div_expr, div_real);
	opensmt_expr ast_tgt = opensmt_mk_gt(ctx, div_expr, opensmt_mk_minus(ctx, div_real, opensmt_mk_num(ctx, 1)));
	opensmt_expr ast_and = opensmt_mk_and_2(ctx, ast_tleq, ast_tgt);
	opensmt_print_expr(ast_and);
	cerr<<endl;
	opensmt_assert(ctx, ast_and);

	opensmt_expr ast = opensmt_mk_minus(ctx, y, opensmt_mk_times_2(ctx, div_expr, z));
	return ast;
}

opensmt_expr Verification::opensmt_mk_ASHR(opensmt_context ctx, opensmt_expr y, int rr, string name, unsigned num){
	opensmt_expr* xt = new opensmt_expr[num];
	vector<opensmt_expr> x;

	for(unsigned i=0;i<num;i++){
		string tname = name+"_t"+ConvertToString(i);
		x.push_back(opensmt_mk_int_var(ctx, tname.c_str(), 0, 1));
		xt[i] = opensmt_mk_times_2(ctx, x[i], opensmt_mk_pow(ctx, opensmt_mk_num(ctx, 2), opensmt_mk_num(ctx, i)));
	}
	opensmt_expr ast_t = opensmt_mk_eq(ctx, y, opensmt_mk_plus(ctx, xt, num));
	opensmt_print_expr(ast_t);
	opensmt_assert(ctx, ast_t);

	delete xt;
	xt = new opensmt_expr[num-rr];
	for(unsigned i=0; i<num-rr; i++){
		xt[i] = opensmt_mk_times_2(ctx, x[i+rr], opensmt_mk_pow(ctx, opensmt_mk_num(ctx, 2), opensmt_mk_num(ctx, i)));
	}

	opensmt_expr ast = opensmt_mk_plus(ctx, xt, num-rr);
	return ast;
}

opensmt_expr Verification::opensmt_mk_SHL(opensmt_context ctx, opensmt_expr y, int rr, string name, unsigned num){
	opensmt_expr* xt = new opensmt_expr[num];
	vector<opensmt_expr> x;

	for(unsigned i=0;i<num;i++){
		string tname = name+"_t"+ConvertToString(i);
		x.push_back(opensmt_mk_int_var(ctx, tname.c_str(), 0, 1));
		xt[i] = opensmt_mk_times_2(ctx, x[i], opensmt_mk_pow(ctx, opensmt_mk_num(ctx, 2), opensmt_mk_num(ctx, i)));
	}
	opensmt_expr ast_t = opensmt_mk_eq(ctx, y, opensmt_mk_plus(ctx, xt, num));
	opensmt_print_expr(ast_t);
	opensmt_assert(ctx, ast_t);

	delete xt;
	xt = new opensmt_expr[num-rr];
	for(unsigned i=0; i<num-rr; i++){
		xt[i] = opensmt_mk_times_2(ctx, x[i], opensmt_mk_pow(ctx, opensmt_mk_num(ctx, 2), opensmt_mk_num(ctx, i+rr)));
	}

	opensmt_expr ast = opensmt_mk_plus(ctx, xt, num-rr);
	return ast;
}

opensmt_expr Verification::opensmt_mk_INT_cmp(opensmt_context ctx, opensmt_expr y, opensmt_expr z, Op_m pvop, string name){
	opensmt_expr cmp;
	switch(pvop){
		case lt:cmp = opensmt_mk_lt(ctx, y, z);break;
		case le:cmp = opensmt_mk_leq(ctx, y, z);break;
		case gt:cmp = opensmt_mk_gt(ctx, y, z);break;
		case ge:cmp = opensmt_mk_geq(ctx, y, z);break;
		case eq:cmp = opensmt_mk_eq(ctx, y, z);break;
		case ne:cmp = opensmt_mk_not(ctx, opensmt_mk_eq(ctx, y, z));break;
		default:errs()<<"Verification::opensmt_mk_INT_cmp error\n";
	}
	string tname = name+"_t";
	opensmt_expr temp = opensmt_mk_int_var(ctx, tname.c_str(), 0, 1);
	opensmt_expr assign0 = opensmt_mk_eq(ctx, temp, opensmt_mk_num(ctx, 0));
	opensmt_expr assign1 = opensmt_mk_eq(ctx, temp, opensmt_mk_num(ctx, 1));
	opensmt_expr assign = opensmt_mk_and_2(ctx, opensmt_mk_eq(ctx, assign1, cmp), opensmt_mk_eq(ctx, opensmt_mk_not(ctx, cmp), assign0));

	opensmt_print_expr(assign);
	opensmt_assert(ctx, assign);
	cerr<< endl;

	return temp;
}

int Verification::getCMP(int rl, int rr, Op_m pvop){
	bool cmp;
	switch(pvop){
		case lt:cmp = (rl<rr);break;
		case le:cmp = (rl<=rr);break;
		case gt:cmp = (rl>rr);break;
		case ge:cmp = (rl>=rr);break;
		case eq:cmp = (rl==rr);break;
		case ne:cmp = (rl!=rr);break;
		default:errs()<<"Verification::getCMP error\n";
	}
	if(cmp)
		return 1;
	return 0;
}

void Verification::get_constraint(Constraint *con, VarTable *table, int time)
{
	Operator op = con->op;
	
	opensmt_expr exprl; 
	opensmt_expr exprr;
	opensmt_expr ast; 
	
	CFG *cfg = table->getCFG();

	ParaVariable lpv,rpv;
	Variable *lv;
	Variable *rv;
	int ID1,ID2;

	switch(op){
		case LT:
			lpv = con->lpvList;
			rpv = con->rpvList;
			if(lpv.isExp||rpv.isExp)
				errs()<<"get_constraint error: isExp1\n";
			lv = lpv.rvar;
			rv = rpv.rvar;
			if((lv->type==PTR&&rv->type!=PTR)||(rv->type==PTR&&lv->type!=PTR))
				errs()<<"get_constraint error: Type is diff\n";
			ID1 = lv->ID;
			ID2 = rv->ID;
			
			if(lv->type == PTR){
				double lval,rval;
				if(!table->getVal(ID1,lval))
					errs()<<"0.LT GetVal error "<<ID1<<"\t"<<cfg->variableList[ID1].name<<"\n";
				if(!table->getVal(ID2,rval))
					errs()<<"1.LT GetVal error "<<ID2<<"\t"<<cfg->variableList[ID2].name<<"\n";
				exprl = opensmt_mk_num(ctx, lval);
				exprr = opensmt_mk_num(ctx, rval);
			}
			else{
				if(lv->type==NUM){
					exprl = opensmt_mk_num_from_string(ctx, lv->name.c_str());
				}
				else
					exprl = table->getX(ID1);
				if(rv->type==NUM){
					exprr = opensmt_mk_num_from_string(ctx, rv->name.c_str());
				}
				else
					exprr = table->getX(ID2);
			}
			ast = opensmt_mk_lt(ctx, exprl, exprr);
			break;
		case LE:	
			lpv = con->lpvList;
			rpv = con->rpvList;
			if(lpv.isExp||rpv.isExp)
				errs()<<"get_constraint error: isExp2\n";
			lv = lpv.rvar;
			rv = rpv.rvar;
			if((lv->type==PTR&&rv->type!=PTR)||(rv->type==PTR&&lv->type!=PTR))
				errs()<<"get_constraint error: Type is diff\n";
			ID1 = lv->ID;
			ID2 = rv->ID;
			
			if(lv->type == PTR){
				double lval,rval;
				if(!table->getVal(ID1,lval))
					errs()<<"0.LE GetVal error "<<ID1<<"\t"<<cfg->variableList[ID1].name<<"\n";
				if(!table->getVal(ID2,rval))
					errs()<<"1.LE GetVal error "<<ID2<<"\t"<<cfg->variableList[ID2].name<<"\n";
				exprl = opensmt_mk_num(ctx, lval);
				exprr = opensmt_mk_num(ctx, rval);
			}
			else{
				if(lv->type==NUM){
					exprl = opensmt_mk_num_from_string(ctx, lv->name.c_str());
				}
				else
					exprl = table->getX(ID1);
				if(rv->type==NUM){
					exprr = opensmt_mk_num_from_string(ctx, rv->name.c_str());
				}
				else
					exprr = table->getX(ID2);
			}
			ast = opensmt_mk_leq(ctx, exprl, exprr);
			break;
		case GT:	
			lpv = con->lpvList;
			rpv = con->rpvList;
			if(lpv.isExp||rpv.isExp)
				errs()<<"get_constraint error: isExp3\n";
			lv = lpv.rvar;
			rv = rpv.rvar;
			if((lv->type==PTR&&rv->type!=PTR)||(rv->type==PTR&&lv->type!=PTR))
				errs()<<"get_constraint error: Type is diff\n";
			ID1 = lv->ID;
			ID2 = rv->ID;
			
			if(lv->type == PTR){
				double lval,rval;
				if(!table->getVal(ID1,lval))
					errs()<<"0.GT GetVal error "<<ID1<<"\t"<<cfg->variableList[ID1].name<<"\n";
				if(!table->getVal(ID2,rval))
					errs()<<"1.GT GetVal error "<<ID2<<"\t"<<cfg->variableList[ID2].name<<"\n";
				exprl = opensmt_mk_num(ctx, lval);
				exprr = opensmt_mk_num(ctx, rval);
			}
			else{
				if(lv->type==NUM){
					exprl = opensmt_mk_num_from_string(ctx, lv->name.c_str());
				}
				else
					exprl = table->getX(ID1);
				if(rv->type==NUM){
					exprr = opensmt_mk_num_from_string(ctx, rv->name.c_str());
				}
				else
					exprr = table->getX(ID2);
			}
			ast = opensmt_mk_gt(ctx, exprl, exprr);
			break;
		case GE:	
			lpv = con->lpvList;
			rpv = con->rpvList;
			if(lpv.isExp||rpv.isExp)
				errs()<<"get_constraint error: isExp4\n";	
			lv = lpv.rvar;
			rv = rpv.rvar;
			if((lv->type==PTR&&rv->type!=PTR)||(rv->type==PTR&&lv->type!=PTR))
				errs()<<"get_constraint error: Type is diff\n";
			ID1 = lv->ID;
			ID2 = rv->ID;
			
			if(lv->type == PTR){
				double lval,rval;
				if(!table->getVal(ID1,lval))
					errs()<<"0.GE GetVal error "<<ID1<<"\t"<<cfg->variableList[ID1].name<<"\n";
				if(!table->getVal(ID2,rval))
					errs()<<"1.GE GetVal error "<<ID2<<"\t"<<cfg->variableList[ID2].name<<"\n";
				exprl = opensmt_mk_num(ctx, lval);
				exprr = opensmt_mk_num(ctx, rval);
			}
			else{
				if(lv->type==NUM){
					exprl = opensmt_mk_num_from_string(ctx, lv->name.c_str());
				}
				else
					exprl = table->getX(ID1);
				if(rv->type==NUM){
					exprr = opensmt_mk_num_from_string(ctx, rv->name.c_str());
				}
				else
					exprr = table->getX(ID2);
			}
			ast = opensmt_mk_geq(ctx, exprl, exprr);
			break;
		case EQ:	
			lpv = con->lpvList;
			rpv = con->rpvList;
			if(lpv.isExp||rpv.isExp)
				errs()<<"get_constraint error: isExp5\n";	
			lv = lpv.rvar;
			rv = rpv.rvar;
			if((lv->type==PTR&&rv->type!=PTR)||(rv->type==PTR&&lv->type!=PTR))
				errs()<<"get_constraint error: Type is diff\n";
			ID1 = lv->ID;
			ID2 = rv->ID;
			
			if(lv->type == PTR){
				double lval,rval;
				if(!table->getVal(ID1,lval))
					errs()<<"0.EQ GetVal error "<<ID1<<"\t"<<cfg->variableList[ID1].name<<"\n";
				if(!table->getVal(ID2,rval))
					errs()<<"1.EQ GetVal error "<<ID2<<"\t"<<cfg->variableList[ID2].name<<"\n";
				exprl = opensmt_mk_num(ctx, lval);
				exprr = opensmt_mk_num(ctx, rval);
			}
			else{
				if(lv->type==NUM){
					exprl = opensmt_mk_num_from_string(ctx, lv->name.c_str());
				}
				else
					exprl = table->getX(ID1);
				if(rv->type==NUM){
					exprr = opensmt_mk_num_from_string(ctx, rv->name.c_str());
				}
				else
					exprr = table->getX(ID2);
			}
			ast = opensmt_mk_eq(ctx, exprl, exprr);
			break;
		case NE:	
			lpv = con->lpvList;
			rpv = con->rpvList;
			if(lpv.isExp||rpv.isExp)
				errs()<<"get_constraint error: isExp5\n";	
			lv = lpv.rvar;
			rv = rpv.rvar;
			if((lv->type==PTR&&rv->type!=PTR)||(rv->type==PTR&&lv->type!=PTR))
				errs()<<"get_constraint error: Type is diff\n";
			ID1 = lv->ID;
			ID2 = rv->ID;
			
			if(lv->type == PTR){
				double lval,rval;
				if(!table->getVal(ID1,lval))
					errs()<<"0.NE GetVal error "<<ID1<<"\t"<<cfg->variableList[ID1].name<<"\n";
				if(!table->getVal(ID2,rval))
					errs()<<"1.NE GetVal error "<<ID2<<"\t"<<cfg->variableList[ID2].name<<"\n";
				exprl = opensmt_mk_num(ctx, lval);
				exprr = opensmt_mk_num(ctx, rval);
			}
			else{
				if(lv->type==NUM){
					exprl = opensmt_mk_num_from_string(ctx, lv->name.c_str());
				}
				else
					exprl = table->getX(ID1);
				if(rv->type==NUM){
					exprr = opensmt_mk_num_from_string(ctx, rv->name.c_str());
				}
				else
					exprr = table->getX(ID2);
			}		
			ast = opensmt_mk_not(ctx, opensmt_mk_eq(ctx, exprl, exprr));
			break;
		case ASSIGN:{
			lpv = con->lpvList;
			rpv = con->rpvList;
			if(lpv.isExp)
				errs()<<"get_constraint error: isExp6\n";
			lv = lpv.rvar;
			
			if(lv->type==PTR){
				if(!rpv.isExp){
					rv = rpv.rvar;
					if(rv->type==PTR){
						double rval = -1;
						if(!table->getVal(rv->ID,rval))
							errs()<<"0.GetVal error "<<rv->ID<<"\t"<<rv->name<<"\t"<<rval<<"\n";
						table->setVal(lv->ID, rval);
					}
					else
						errs()<<"get_constraint error: PTR rv->type error1\n";
				}
				else{
					Op_m pvop = rpv.op;
					Variable *rvl;
					Variable *rvr;
					double rvlval,rvrval,val;
					int allocaID,addr,aliasID;
					switch(pvop){
						case ADD:
							rvl = rpv.lvar;
							rvr = rpv.rvar;
							if(!table->getVal(rvl->ID,rvlval))
								errs()<<"1.GetVal error "<<rvl->ID<<"\t"<<rvl->name<<"\n";
							if(rvr->type==NUM){
								rvrval = ConvertToDouble(rvr->name);
							}
							else{
								if(!table->getVal(rvr->ID,rvrval))
									errs()<<"2.GetVal error "<<rvr->ID<<"\t"<<rvr->name<<"\n";
							}
							val = rvlval+rvrval;
							table->setVal(lv->ID, val);
							break;
						case GETPTR:
							rvl = rpv.lvar;
							rvr = rpv.rvar;
							if(!table->getVal(rvl->ID,rvlval))
								errs()<<"3.GetVal error "<<rvl->ID<<"\t"<<lv->name<<"\n";
							if(rvr->type==NUM)
								rvrval = ConvertToDouble(rvr->name);
							else{
								if(!table->getVal(rvr->ID,rvrval))
									errs()<<"4.GetVal error "<<rvr->ID<<"\t"<<*con<<"\n";
							}
							addr = (int)(rvlval+rvrval);
							if(!table->getVal(addr, val))
								errs()<<"5.GetVal error "<<addr<<"\t"<<*con<<"\n";
							table->setVal(lv->ID, val);
							break;
						case ADDR:
							rv = rpv.rvar;
							table->setVal(lv->ID, rv->ID);
							if(!table->getVal(lv->ID,rvrval))
								errs()<<"ADDR error "<<*con<<"\t"<<rv->ID<<"\n";
							break;
						case STORE:
							if(!table->getVal(lv->ID, val))
								errs()<<"6.GetVal error "<<lv->ID<<"\t"<<lv->name<<"\n";
							allocaID = (int)val;
							rv = rpv.rvar;
							table->setAlias(allocaID, rv->ID);
							break;
						case LOAD:
							rv = rpv.rvar;
							if(!table->getVal(rv->ID, val))
								errs()<<"0.LOAD GetVal error "<<rv->ID<<"\t"<<cfg->variableList[rv->ID].name<<"\n";	
//
							allocaID = (int)val;
//							errs()<<"0.2 LOAD : "<<*con<<"\n";
							aliasID = table->getAlias(allocaID);
//							errs()<<"0.3 LOAD : "<<*con<<"\n";
							if(!table->getVal(aliasID, val))
								errs()<<"1.LOAD GetVal error "<<aliasID<<"\t"<<cfg->variableList[aliasID].name<<"\n";	
//							errs()<<"0.4 LOAD : "<<*con<<"\n";
							table->setVal(lv->ID, val);
							break;
						case ALLOCA:
							allocaID = table->alloca();
							table->setVal(lv->ID, allocaID);
							break;
						default:	
							errs()<<"get_constraint error: PTR rpv.op error "<<*con<<"\n";
					}
				}
				return;
			}
			else if(lv->type==INT||lv->type==FP){
				if(time>1)
					table->setX(lv->ID, time, lv->type);
				exprl = table->getX(lv->ID);

				if(!rpv.isExp){
					rv = rpv.rvar;
					if(rv->type==NUM){
						exprr = opensmt_mk_num_from_string(ctx, rv->name.c_str());
						double val = ConvertToDouble(rv->name);
						table->setVal(lv->ID, val);
					}
					else if(rv->type==INT || rv->type==FP){
						exprr = table->getX(rv->ID);
						double val;
						if(lv->type==INT && rv->type==FP){
							opensmt_expr ast_tleq = opensmt_mk_leq(ctx, exprl, exprr);
							opensmt_expr ast_tgt = opensmt_mk_gt(ctx, exprl, opensmt_mk_minus(ctx, exprr, opensmt_mk_num(ctx, 1)));
							opensmt_expr ast_and = opensmt_mk_and_2(ctx, ast_tleq, ast_tgt);
							opensmt_print_expr(ast_and);
							cerr<<endl;
							opensmt_assert(ctx, ast_and);
							if(table->getVal(rv->ID, val))
								table->setVal(lv->ID, (int)val);
							return;
						}
						else{
							if(table->getVal(rv->ID, val))
								table->setVal(lv->ID, val);
						}
					}
					else
						errs()<<"get_constraint error: DATA rv->type error\n";
				}
				else{
					Op_m pvop = rpv.op;
					Variable *rvl;
					Variable *rvr;
					double rl=0;
					double rr=0;
					double val=0;
					opensmt_expr y;
					opensmt_expr z;
					string name = lv->name;
					bool treat = true;
					switch(pvop){
						case LOAD:
//							errs()<<"1.1 LOAD : "<<*con<<"\n";
							rvr = rpv.rvar;
							if(!table->getVal(rvr->ID, val))
								errs()<<"2.LOAD GetVal error "<<rvr->ID<<"\t"<<cfg->variableList[rvr->ID].name<<"\n";	
							rl = (int)val;
							rr = table->getAlias(rl);
							treat = table->getVal(rr, val);
							if(treat)
								table->setVal(lv->ID, val);
							exprr = table->getX(rr);
							break;
						case AND:
							rvl = rpv.lvar;
							rvr = rpv.rvar;
							y = getExpr(rvl, treat, rl, table);
							z = getExpr(rvr, treat, rr, table);		
							exprr = opensmt_mk_AND(ctx, y, z, name, 32);	
							if(treat)
								table->setVal(lv->ID, (int)rl&(int)rr);
							break;
						case NAND:
							rvl = rpv.lvar;
							rvr = rpv.rvar;
							y = getExpr(rvl, treat, rl, table);
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_NAND(ctx, y, z, name, 32);
							if(treat)
								table->setVal(lv->ID, ~((int)rl&(int)rr));
							break;
						case OR:
							rvl = rpv.lvar;
							rvr = rpv.rvar;
							y = getExpr(rvl, treat, rl, table);
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_OR(ctx, y, z, name, 32);
							if(treat)
								table->setVal(lv->ID, (int)rl|(int)rr);
							break;
						case XOR:
							rvl = rpv.lvar;
							rvr = rpv.rvar;
							y = getExpr(rvl, treat, rl, table);
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_XOR(ctx, y, z, name, 32);
							if(treat)
								table->setVal(lv->ID, (int)rl^(int)rr);
							break;
						case SREM:
							rvl = rpv.lvar;
							rvr = rpv.rvar;
							y = getExpr(rvl, treat, rl, table);
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_SREM(ctx, y, z, name);
							if(treat)
								table->setVal(lv->ID, (int)rl%(int)rr);
							break;
						case ASHR:
							rvl = rpv.lvar;
							rvr = rpv.rvar;
							z = getExpr(rvr, treat, rr, table);
							if(!treat)
								errs()<<"ASHR error: invalid z value\n";
							y = getExpr(rvl, treat, rl, table);
							if(rr<0)
								exprr = opensmt_mk_SHL(ctx, y, -(int)rr, name, 32);
							else
								exprr = opensmt_mk_ASHR(ctx, y, (int)rr, name, 32);
							if(treat)
								table->setVal(lv->ID, (int)rl>>(int)rr);
							break;
						case SHL:
							rvl = rpv.lvar;
							rvr = rpv.rvar;
							z = getExpr(rvr, treat, rr, table);
							if(!treat)
								errs()<<"SHL error: invalid z value\n";
							y = getExpr(rvl, treat, rl, table);
							if(rr>=0)
								exprr = opensmt_mk_SHL(ctx, y, (int)rr, name, 32);
							else
								exprr = opensmt_mk_ASHR(ctx, y, -(int)rr, name, 32);
							if(treat)
								table->setVal(lv->ID, (int)rl<<(int)rr);
							break;
						case ADD:
							rvl = rpv.lvar;
							rvr = rpv.rvar;
							y = getExpr(rvl, treat, rl, table);
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_plus_2(ctx, y, z);
							if(treat)
								table->setVal(lv->ID, rl+rr);
							break;
						case SUB:
							rvl = rpv.lvar;
							rvr = rpv.rvar;
							y = getExpr(rvl, treat, rl, table);
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_minus(ctx, y, z);
							if(treat)
								table->setVal(lv->ID, rl-rr);
							break;
						case TAN:
							rvr = rpv.rvar;
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_tan(ctx, z);
							if(treat)
								table->setVal(lv->ID, tan(rr));
							break;
						case ATAN:
							rvr = rpv.rvar;
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_atan(ctx, z);
							if(treat)
								table->setVal(lv->ID, atan(rr));
							break;
						case ATAN2:
							rvl = rpv.lvar;
							rvr = rpv.rvar;
							y = getExpr(rvl, treat, rl, table);
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_atan2(ctx, y, z);
							if(treat)
								table->setVal(lv->ID, atan2(rl, rr));
							break;
						case SIN:
							rvr = rpv.rvar;
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_sin(ctx, z);
							if(treat)
								table->setVal(lv->ID, sin(rr));
							break;
						case ASIN:
							rvr = rpv.rvar;
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_asin(ctx, z);
							if(treat)
								table->setVal(lv->ID, asin(rr));
							break;
						case COS:
							rvr = rpv.rvar;
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_cos(ctx, z);
							if(treat)
								table->setVal(lv->ID, cos(rr));
							break;
						case ACOS:
							rvr = rpv.rvar;
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_acos(ctx, z);
							if(treat)
								table->setVal(lv->ID, acos(rr));
							break;
						case SQRT:
							rvr = rpv.rvar;
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_pow(ctx, z, opensmt_mk_num(ctx, 0.5));
							if(treat)
								table->setVal(lv->ID, sqrt(rr));
							break;
						case POW:
							rvl = rpv.lvar;
							rvr = rpv.rvar;
							y = getExpr(rvl, treat, rl, table);
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_pow(ctx, y, z);
							if(treat)
								table->setVal(lv->ID, powf(rl,rr));
							break;
						case LOG:
							rvr = rpv.rvar;
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_log(ctx, z);
							if(treat)
								table->setVal(lv->ID, log(rr));
							break;
						case LOG10:
							rvr = rpv.rvar;
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_div(ctx, opensmt_mk_log(ctx, z),opensmt_mk_log(ctx, opensmt_mk_num(ctx, 10)));
							if(treat)
								table->setVal(lv->ID, log10(rr));
							break;
						case ABS:
							rvr = rpv.rvar;
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_abs(ctx, z);
							if(treat)
								table->setVal(lv->ID, fabs(rr));
							break;
						case EXP:
							rvr = rpv.rvar;
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_exp(ctx, z);
							if(treat)
								table->setVal(lv->ID, exp(rr));
							break;
						case SINH:
							rvr = rpv.rvar;
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_sinh(ctx, z);
							if(treat)
								table->setVal(lv->ID, sinh(rr));
							break;
						case COSH:
							rvr = rpv.rvar;
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_cosh(ctx, z);
							if(treat)
								table->setVal(lv->ID, cosh(rr));
							break;
						case TANH:
							rvr = rpv.rvar;
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_tanh(ctx, z);
							if(treat)
								table->setVal(lv->ID, tanh(rr));
							break;
						case MUL:
							rvl = rpv.lvar;
							rvr = rpv.rvar;
							y = getExpr(rvl, treat, rl, table);
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_times_2(ctx, y, z);
							if(treat)
								table->setVal(lv->ID, rl*rr);
							break;
						case DIV:
							rvl = rpv.lvar;
							rvr = rpv.rvar;
							y = getExpr(rvl, treat, rl, table);
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_div(ctx, y, z);
							if(treat&&rr!=0)
								table->setVal(lv->ID, rl/rr);
							break;
						case lt:case le:case gt:case ge:case eq:case ne:
							rvl = rpv.lvar;
							rvr = rpv.rvar;
							y = getExpr(rvl, treat, rl, table);
							z = getExpr(rvr, treat, rr, table);
							exprr = opensmt_mk_INT_cmp(ctx, y, z, pvop, name);
							if(treat)
								table->setVal(lv->ID, getCMP(rl, rr ,pvop));
							break;
						default:
							errs()<<"get_constraint error: DATA rpv.op error "<<*con<<"\n";
					}
				}
			}
			else
				errs()<<"get_constraint error: lv->type error\n";
			ast = opensmt_mk_eq(ctx, exprl, exprr);
			break;
		}
	}

	opensmt_print_expr(ast);
    opensmt_assert(ctx, ast);
	cerr<< endl;
}


void Verification::encode_path(CFG* ha, vector<int> patharray)
{
	int var_num = ha->variableList.size();
	vector<int> var(var_num,-1);
	map<int, double> value;
	double pre = opensmt_get_precision(ctx);
	cerr<<"Precision is "<<pre<<endl;

	table = new VarTable(ctx, ha);

	int state_num =	 (patharray.size()+1)/2;
	int total_state  = ha->stateList.size()+ ha->transitionList.size();
	vector<int> repeat(total_state,0);
	
	for (int j= 0;j<state_num; j++)
	{	
		State* st=ha->searchState(patharray[2*j]);
		assert(st!=NULL);
		cerr<<st->name<<":"<<endl;
		for(unsigned m=0;m<st->consList.size();m++)
		{
			Constraint* con = &st->consList[m];
			int ID = patharray[2*j];
			get_constraint(con, table, repeat[ID] );

		}
		repeat[patharray[2*j]]+=1;
		
		if(j!=state_num-1)	
		{
			Transition* pre=ha->searchTransition(patharray[2*j+1]);
			assert(pre!=NULL);
			cerr<<pre->name<<":"<<endl;
	
			for(unsigned m=0;m<pre->guardList.size();m++)
			{
				Constraint* con = &pre->guardList[m];
				int ID = patharray[2*j+1];
				get_constraint(con, table, repeat[ID] );
			}	
			repeat[patharray[2*j+1]]+=1;

		}
	}
//	cerr<<"Path encode complete~~~~~~~~~~~~~~~~~~ "<<endl;

}

/*******************************class BoundedVerification****************************************/
BoundedVerification::BoundedVerification(CFG* aut, int bound, vector<int> target){
	this->cfg=aut;
	this->bound=bound;
	this->target=target;
    result = false;
    reachEnd = false;
	num_of_path=0;
}

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

void BoundedVerification::DFS(int intbound,int bound,int start, int end){
    if(bound==0||result==true)
        return;
    
    reachEnd = false;
    int reduntsize=path.size()-2*(intbound-bound);
	if(reduntsize!=0){
		int temp=path.back();
		for(int m=0;m<reduntsize+1;m++)
			path.pop_back();
		path.push_back(temp);
	}

	path.push_back(start);
    
    if(start==end){
        reachEnd = true;
        target_name=cfg->getNodeName(end);
    }

	if(verify.check(cfg, path)){   //the path is feasible, terminate
		num_of_path++;

		if(reachEnd){
			reachPath=get_name(cfg,path);
            for(unsigned i=0;i<path.size();i++){
                witPath.push_back(path[i]);
            }
			result = true;
            return;
        }
		else {
			for(unsigned int i=0;i<cfg->searchState(start)->transList.size();i++){
				State *s = cfg->searchState(start)->transList[i]->toState;
				if(s==NULL) continue;
				path.push_back(cfg->searchState(start)->transList[i]->ID);
				DFS(intbound,bound-1,s->ID,end);
			}
		}
	}

}

/* Bounded reachability analysis return false:unreachable true:reachable */

bool BoundedVerification::check(double &time,string check){
	cfg->print();

	int line;
	if(target.size()==0){
		errs()<<"Has no div exceptions~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
		return true;
	}

    for(int i=0;i<(int)target.size();i++)
        errs()<<"target["<<i<<"]:"<<cfg->stateList[target[i]].name<<"("<<target[i]<<")\n";
    
    for(int i=0;i<(int)target.size();i++){
        result = false;
        reachEnd = false;
        path.clear();
        witPath.clear();
        errs()<<"target["<<i<<"]:"<<cfg->stateList[target[i]].name<<"("<<target[i]<<")\n";
		DFS(bound,bound,cfg->initialState->ID,target[i]);
		int targetID = target[i];
		string name = cfg->stateList[targetID].name;
		if(name.at(0)=='q')
			line = cfg->stateList[targetID].locList[0];
		errs()<<"target["<<i<<"]:from "<<cfg->initialState->name<<"("<<cfg->initialState->ID<<") to "<<cfg->stateList[targetID].name<<"("<<targetID<<")\n";
		if( result ){
			errs()<<"\nNumber of path checked:"<<num_of_path<<"\n";
			errs()<<"line "<<line<<" in state "<<target_name<<" is reachable\n";
			verify.print_sol(cfg, cfg->mainInput);
			errs()<<"Witness:\n";
			for(unsigned i=0; i<witPath.size();i++){
				int id = witPath[i];
				State *s = cfg->searchState(id);
				assert(s!=NULL);
				cerr<<"\t"<<s->name<<":";
				cerr<<"\tLocLine:";
				for(unsigned j=0;j<s->locList.size();j++)
					cerr<<s->locList[j]<<";";
				cerr<<"\n";
				if(i<witPath.size()-1){
					Transition *t = cfg->searchTransition(witPath[++i]);
					assert(t!=NULL);
					cerr<<"\t"<<t->name<<"\n";
				}
			}
		}
		else{
			errs()<<"Number of path checked:"<<num_of_path<<"\n";
			errs()<<"line "<<line<<" is not reachable under bound "<<bound<<" when check="<<check<<"\n";
		}
		time = verify.getTime();
		errs() << "Dreal Time: \t" << ConvertToString(verify.getTime()) << "ms\n";
	}

	return result;
}


string BoundedVerification::get_path_name(CFG *cfg,vector<int> path){
	string name="";
	for(unsigned i=0;i<path.size();i++){
		if(i%2==0){
		name +=cfg->getNodeName(path[i]);

		if(i != path.size()-1)
		  name += "^";
	}
	}
	if(cfg->searchState(path[path.size()-1])->locList.empty()){
		name=name.substr(0,name.length()-5);
	}
	return name;
}

