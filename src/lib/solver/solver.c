#include "solver.h"

const struct Solver *const solvers[] = {
    &iterative_solver,
    &karp_li_parallel_solver,
    &karp_li_solver,
    &aggarwal_solver,
    &aggarwal_parallel_solver_on_chains,
    &aggarwal_parallel_solver,
    &aggarwal_parallel_solver_on_neutral,
};
const int solvers_num = sizeof(solvers) / sizeof(solvers[0]);
