#include <stdlib.h>

#include "../lib/linear_solver/linear_solver.h"
#include "./linear_solver/test_linear_solvers.h"

const struct LinearSolver *linear_solvers[] = {
    &(struct LinearSolver){
        .solve = linear_solve_iterative,
        .params = NULL,
        .name = "Iterative solver",
    },
    &(struct LinearSolver){
        .solve = linear_solve_karp_li,
        .params = NULL,
        .name = "Karp-Li solver",
    },
    &(struct LinearSolver){
        .solve = linear_solve_karp_li_parallel,
        .params = &(KarpLiParallelParams){.thread_num = 3},
        .name = "Karp-Li solver parallel",
    },
    &(struct LinearSolver){
        .solve = linear_solve_aggarwal,
        .params = NULL,
        .name = "Aggarwal solver",
    },
    &(struct LinearSolver){
        .solve = linear_solve_aggarwal_parallel,
        .params = &(AggarwalParallelOnChainsParams){.thread_num = 3},
        .name = "Aggarwal solver parallel",
    },
    &(struct LinearSolver){
        .solve = linear_solve_aggarwal_parallel_on_chains,
        .params = &(AggarwalParallelOnChainsParams){.thread_num = 3},
        .name = "Aggarwal solver parallel on chains",
    },
    &(struct LinearSolver){
        .solve = linear_solve_aggarwal_parallel_on_neutral,
        .params = &(AggarwalParallelOnNeutralParams){.thread_num = 4},
        .name = "Aggarwal solver parallel on neutral",
    },
};

const int linear_solvers_num =
    sizeof(linear_solvers) / sizeof(linear_solvers[0]);

int main(void) {
  if (!test_linear_solvers(linear_solvers, linear_solvers_num)) {
    exit(EXIT_FAILURE);
  };
  return 0;
}
