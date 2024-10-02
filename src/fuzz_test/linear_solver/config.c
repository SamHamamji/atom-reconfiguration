#include <stdlib.h>

#include "./config.h"

static const struct LinearSolver *linear_solvers[] = {
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

static struct Interval *interval_generator(int length, int imbalance) {
  return interval_factory.generate_compact_target_region(length, imbalance);
}

struct LinearSolversFuzzTestConfig linear_solvers_config = {
    .length_range = {.start = 0, .exclusive_end = 1000},
    .interval_generator = interval_generator,
    .linear_solvers = linear_solvers,
    .linear_solvers_num = sizeof(linear_solvers) / sizeof(linear_solvers[0]),
    .time_limit_in_seconds = 5.0,
};
