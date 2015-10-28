/**
 *Extract multiple/all minimal unsatisfiable subsets from an unsat constraint set
 *Author: Xie Dingbao
 *2013/10/11
 */

#ifndef _BACH_MUSSANALYZER
#define  _BACH_MUSSANALYZER
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "z3++.h"
#include "general.h"

extern int UC_LEVEL;
#define MUS_LIMIT 50

class SubsetSolver{        
        unsigned n;
        z3::context& c;
        z3::solver s;
        z3::expr_vector constraints;
public:
        SubsetSolver(z3::context& _c, z3::expr_vector _constraints);
        z3::expr c_var(int i);
        bool check_subset(std::vector<int> seed);
        z3::expr_vector to_c_lits(std::vector<int> seed);
        std::vector<int> seed_from_core();
        std::vector<int> shrink(std::vector<int> seed);
        z3::expr get_constraint(int i);
        int get_id(z3::expr x);
        unsigned size(){return n;}
};

class MapSolver{
        z3::context c;
        z3::solver s;
        int n;
        std::vector<int> all_n;
public:
        MapSolver(int n);
        std::vector<int> next_seed();
        void block_up(std::vector<int> frompoint);
        void block_down(std::vector<int> frompoint);
};


        
        

#endif


