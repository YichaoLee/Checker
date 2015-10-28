#include "MUSSAnalyzer.h"
using namespace z3;
using namespace std;


static void remove(vector<int>& seed, int c){
    for(vector<int>::iterator it=seed.begin();it!=seed.end();++it){
		if(*it == c){
			seed.erase(it);
            break;
		}
    }
}

int SubsetSolver::get_id(expr x){
        func_decl v=x.decl();
        return atoi(v.name().str().c_str());
}

SubsetSolver::SubsetSolver(context& _c, expr_vector _constraints):c(_c),s(_c),constraints(_constraints){
        n = constraints.size();
        params p(c);
        p.set(":unsat-core",true);
        p.set(":auto-config",false);
        s.set(p);
        for(unsigned i=0;i<n;i++)
            s.add(implies(c_var(i), constraints[i]));
}

expr SubsetSolver::c_var(int i){
        char ch[256];
        sprintf(ch,"%i",i);
        expr v=c.bool_const(ch);
        return v;
}

expr SubsetSolver::get_constraint(int i){
        return constraints[i];
}

bool SubsetSolver::check_subset(vector<int> seed){
        expr_vector assumptions=to_c_lits(seed);
//        cout<<"assumptions: "<<assumptions<<endl;
        return (s.check(assumptions) == sat);
}

expr_vector SubsetSolver::to_c_lits(vector<int> seed){
        expr_vector lits(c);
        for(unsigned i=0;i<seed.size();i++)
                lits.push_back(c_var(seed[i]));
        return lits;
}

vector<int> SubsetSolver::seed_from_core(){
        vector<int> seed;
        expr_vector core = s.unsat_core();
        for(unsigned i=0;i<core.size();i++)
            seed.push_back(get_id(core[i]));
        return seed;
}

vector<int> SubsetSolver::shrink(vector<int> seed){
        vector<int> core = seed_from_core();
        for(unsigned i=0;i<seed.size();i++){
            remove(core, seed[i]);
            if(s.check(to_c_lits(core)) == sat)
				core.push_back(seed[i]);
        }
        return core;
}

MapSolver::MapSolver(int _n):s(c),n(_n){
        for(int i=0;i<n;i++)
          all_n.push_back(i);
}

vector<int> MapSolver::next_seed(){
        vector<int> seed;
        if(s.check() == sat){
                seed = all_n;
                model m=s.get_model();
                for(unsigned i=0;i<m.size();i++){
                        func_decl v=m[i];
                        expr value=m.get_const_interp(v);
                        if(value.is_bool() && eq(value,c.bool_val(false))){
                                remove(seed,atoi(v.name().str().c_str()));
                        }
                }
        }
        return seed;
}

void MapSolver::block_up(vector<int> frompoint){
        expr exp(c);
        for(unsigned i=0;i<frompoint.size();i++){
                expr exp1 = !c.bool_const(int2string(frompoint[i]).c_str());
                if(i == 0)
                  exp=exp1;
                else
                  exp=exp || exp1;
        }
        if(frompoint.size()!=0)
                s.add(exp);
}

void MapSolver::block_down(vector<int> frompoint){
        expr exp(c);
        bool initial = true;
        for(unsigned i=0;i<all_n.size();i++){
                unsigned j;
                for(j=0;j<frompoint.size();j++){
                        if(all_n[i] == frompoint[j])
                          break;
                }
                if(j == frompoint.size()){
                        expr exp1 = c.bool_const(int2string(all_n[i]).c_str());
                        if(initial){
                                exp = exp1;
                                initial = false;
                        }
                        else
                                exp = exp||exp1;
                }
        }
        if(!initial){
                s.add(exp);
        }
}
