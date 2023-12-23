#pragma once

#include "../interval/interval.h"
#include "../mapping/mapping.h"

typedef struct Mapping *SolverFunction(const struct Interval *interval);

struct Solver {
  const char *const name;
  SolverFunction *const solve;
};

extern const struct Solver iterative_solver;
extern const struct Solver karp_li_solver;
extern const struct Solver karp_li_parallel_solver;
extern const struct Solver aggarwal_solver;
extern const struct Solver aggarwal_parallel_solver_on_chains;
extern const struct Solver aggarwal_parallel_solver;
extern const struct Solver aggarwal_parallel_solver_on_neutral;

extern const struct Solver *const solvers[];
extern const int solvers_num;
