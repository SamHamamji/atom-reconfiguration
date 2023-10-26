#include "solvers.h"

const struct Solver *const solvers[] = {
    &iterative_solver,
    &iterative_solver_naive,
    &karp_li_solver,
    &karp_li_solver_optimized,
    &karp_li_solver_naive,
    &karp_li_solver_optimized_naive,
    &parallel_solver,
    &parallel_solver_padded,
    &parallel_solver_fixed_threads,
};
const int solvers_num = sizeof(solvers) / sizeof(solvers[0]);
