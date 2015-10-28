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

class Verification{

	string smt;
	opensmt_context ctx;

	void smt_push(string str);
	void smt_declare(int ID);
	void smt_assert(string str);
	void get_constraint(Constraint *con, vector<opensmt_expr> &x, vector<int> &var, bool repeat);
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
	BoundedVerification(CFG* aut,int bound,int target);
	bool check(int line,string check);
	~BoundedVerification(){}
private:
	CFG* cfg;
	int bound;
	string reachPath;
	int* traversedPath;
	int num_of_path;
	int target;
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
	void DFS(int intbound,int bound,int start,int end);
};


#endif
