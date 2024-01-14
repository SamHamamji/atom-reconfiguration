#include <stdlib.h>

#include "../lib/linear_solver/linear_solver.h"
#include "./linear_solver/test_linear_solvers.h"
#include "./red_rec/test_red_rec.h"

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
  bool success = true;
  success = success && test_linear_solvers(linear_solvers, linear_solvers_num);
  success = success && test_red_rec(&(struct RedRecUnitTestConfig){
                           .max_width = 30,
                           .min_width = 4,
                           .max_height = 30,
                           .min_height = 4,
                           .test_case_num = 2000,
                       });

  if (!success) {
    exit(EXIT_FAILURE);
  }
  return 0;
}
