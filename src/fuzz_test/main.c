#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lib/grid/grid.h"
#include "../lib/linear_solver/linear_solver.h"
#include "../lib/utils/max_min.h"
#include "../lib/utils/seed.h"
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

static struct Interval *interval_generator(int length, int imbalance) {
  return interval_factory.generate_compact_target_region(length, imbalance);
}

static struct LinearSolversFuzzTestConfig linear_solvers_config = {
    .length_range = {.start = 0, .exclusive_end = 30},
    .interval_generator = interval_generator,
    .linear_solvers = linear_solvers,
    .linear_solvers_num = sizeof(linear_solvers) / sizeof(linear_solvers[0]),
    .time_limit_in_seconds = 5.0,
};

static struct Grid *grid_generator(int width, int height) {
  return grid_factory.generate_compact_target_region(width, height,
                                                     rand() % max(height, 1));
}

static struct RedRecFuzzTestConfig red_rec_config = {
    .width_range = {0, 35},
    .height_range = {0, 35},
    .time_limit_in_seconds = 5.0,
    .grid_generator = grid_generator,
};

int main(void) {
  seed_set_to_time();

  if (!fuzz_test_linear_solvers(linear_solvers_config)) {
    return EXIT_FAILURE;
  }

  seed_set_to_time();
  if (!fuzz_test_red_rec(red_rec_config)) {
    return EXIT_FAILURE;
  }
  printf("🟢 All tests passed.\n");
}
