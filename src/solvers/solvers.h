#pragma once

#include "solver.h"

extern const struct Solver iterative_solver;
extern const struct Solver karp_li_solver;
extern const struct Solver parallel_solver;
extern const struct Solver aggarwal_solver;

extern const struct Solver *const solvers[];
extern const int solvers_num;
