#ifndef _verification_h
#define _verification_h
#include "CFG.h"
#include "general.h"
#include <stdio.h>
#include <cstdlib>
#include "minisat/core/Solver.h"
#include "z3++.h"
#include "MUSSAnalyzer.h"
#include <sstream>
#include <cstring>
#include "opensmt_c.h"
#include <fstream>
#include "math.h"

extern int smt2set_in   (FILE *);
extern int smt2parse    ();
extern int m_argc;
extern char ** m_argv;


class IndexPair{
public:
	int start;
	int end;
	IndexPair(int _start,int _end):start(_start),end(_end){}
	bool contain(IndexPair& index){return start>=index.start && end<=index.end;}
	void print(){errs()<<"("<<start<<","<<end<<")";}
};


class VarTable{
private:
	opensmt_context &ctx;
	int var_num;
	map<int, double> varVal;
	map<int, int> alias;
	map<int, int> ptr;
	vector<opensmt_expr> x;
public:
	VarTable(opensmt_context &c, int num):ctx(c), var_num(num){
		for(unsigned i=0;i<var_num;i++)
		{
			x.push_back(opensmt_mk_unbounded_real_var(ctx, ("x_"+int2string(i)).c_str()));
		}
	}
	~VarTable(){varVal.clear();alias.clear();ptr.clear();}
	void pushX(int ID){
		setAlias(ID, var_num);
		x.push_back(opensmt_mk_unbounded_real_var(ctx, ("x_"+int2string(var_num)).c_str()));
		var_num++;
	}

	void setAlias(int ID1, int ID2){
		int count = alias.count(ID2);
		if(count == 0)
			alias[ID1] = ID2;
		else{
			int id = alias[ID2];
			if(id!=ID1)
				alias[ID1] = id;
			else
				return;
		}	
		int id = alias[ID1];
		count = varVal.count(id);
		if(count == 1)
			varVal[ID1] = varVal[id];
		
		
		
	}
	void setVal(int ID, double val){

		varVal[ID] = val;

		for(map<int, int>::iterator iter = alias.begin();iter!=alias.end();iter++){
			if(iter->second == ID){
				int aID = iter->first;
				varVal[aID] = val;
			}
		}
		
		for(map<int, int>::iterator iter = ptr.begin();iter!=ptr.end();iter++){
			if(iter->second == ID){
				int aID = iter->first;
				varVal[aID] = val;
			}
		}

	}
	void setPTR(int ID1, int ID2){
		ptr[ID1] = ID2;
		int count = varVal.count(ID1);
		if(count)
			varVal[ID2] = varVal[ID1];
	}
	int getNum(){
		return var_num;	
	}
	opensmt_expr getX(int ID){
		return x[ID];
	}
	bool getalias(int ID, int &a){
		int count = alias.count(ID);
		if(count){
			a = alias[ID];
			return true;
		}
		a = ID;
		return false;
	}
	bool getVal(int ID, double &v){
		int count = varVal.count(ID);
		if(count){
			v = varVal[ID];
			return true;
		}
		return false;
	}
	bool getPTR(int ID, int &p){
		int count = ptr.count(ID);
		if(count){
			p = ptr[ID];
			return true;
		}
		p = ID;
		return false;
	}
	map<int, double> getValmap(){
	   	return varVal;
	}
	map<int, int> getAliasmap(){
	   	return alias;
	}
	map<int, int> getPTRmap(){
	   	return ptr;
	}
};

class Verification{

	string smt;
	opensmt_context ctx;

	void smt_push(string str);
	void smt_declare(int ID);
	void smt_assert(string str);
	void get_constraint(Constraint *con, vector<opensmt_expr> &x, vector<int> &var, map<int, double> &value, bool repeat);
	void get_constraint1(Constraint *con, VarTable *table, bool repeat);
	string get_pvList(ParaVariable pvList);
	string get_Variable(Variable *var);
	void encode_path(CFG* ha, vector<int> patharray);

	std::vector<IndexPair> index_cache; 
	std::vector<IndexPair> core_index; 	
	void clear(){index_cache.clear();core_index.clear();smt="";opensmt_del_context(ctx);ctx = opensmt_mk_context(qf_nra);}
public:
	Verification(){
		opensmt_init();
	    	ctx = opensmt_mk_context(qf_nra);
	}
	bool check(CFG* ha, vector<int> path);
	vector<IndexPair> get_core_index(){return core_index;}
	~Verification(){opensmt_del_context(ctx);}
};

class BoundedVerification{
public:
	BoundedVerification(CFG* aut,int bound,vector<int> target);
	bool check(int line,string check);
	~BoundedVerification(){}
private:
	CFG* cfg;
	int bound;
	string reachPath;
	string target_name;
	int* traversedPath;
	int num_of_path;
	vector<int> target;
	int stNum;
	Verification verify;
	Minisat::Solver s;
	bool solve();
	void encode_graph();
	Minisat::Lit var(const int loop, const int ID);
	void decode(int code, int& loop, int& ID);
	string get_path_name(CFG *cfg,vector<int> path);
	void block_path(int number,CFG *cfg,vector<int> path);
	vector<int> decode_path();
	void DFS(int intbound,int bound,int start,vector<int> end);
};




#endif
