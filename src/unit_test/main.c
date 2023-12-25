#include <stdlib.h>

#include "../lib/solver/solver.h"
#include "./solver/test_solvers.h"

const struct Solver *default_solvers[] = {
    &(struct Solver){
        .solve = iterative_solver_function,
        .params = NULL,
        .name = "Iterative solver",
    },
    &(struct Solver){
        .solve = karp_li_solver_function,
        .params = NULL,
        .name = "Karp-Li solver",
    },
    &(struct Solver){
        .solve = karp_li_parallel_solver_function,
        .params = &(KarpLiParallelParams){.thread_num = 3},
        .name = "Karp-Li solver parallel",
    },
    &(struct Solver){
        .solve = aggarwal_solver_function,
        .params = NULL,
        .name = "Aggarwal solver",
    },
    &(struct Solver){
        .solve = aggarwal_parallel_solver_function,
        .params = &(AggarwalParallelOnChainsParams){.thread_num = 4},
        .name = "Aggarwal solver parallel",
    },
    &(struct Solver){
        .solve = aggarwal_parallel_on_chains_solver_function,
        .params = &(AggarwalParallelOnChainsParams){.thread_num = 3},
        .name = "Aggarwal solver parallel on chains",
    },
    &(struct Solver){
        .solve = aggarwal_parallel_on_neutral_solver_function,
        .params = &(AggarwalParallelOnNeutralParams){.thread_num = 4},
        .name = "Aggarwal solver parallel on neutral",
    },
};

const int default_solvers_num =
    sizeof(default_solvers) / sizeof(default_solvers[0]);

int main(void) {
  if (!test_solvers(default_solvers, default_solvers_num)) {
    exit(EXIT_FAILURE);
  };
  return 0;
}
