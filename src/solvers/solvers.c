#include "solvers.h"

const struct Solver *const solvers[] = {
    &iterative_solver,
    &karp_li_solver,
};
const int solvers_num = sizeof(solvers) / sizeof(solvers[0]);
