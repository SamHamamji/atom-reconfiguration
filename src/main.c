#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "./lib/interval/interval.h"
#include "./lib/mapping/mapping.h"
#include "./lib/solver/solver.h"

struct Config {
  int size;
  int imbalance_percentage;
  int solver_num;
  const struct Solver *solvers[];
};

static const struct Config config = {
    .size = 10000000,
    .imbalance_percentage = 15,
    .solvers = {&aggarwal_solver, &aggarwal_parallel_solver,
                &aggarwal_parallel_solver_on_chains,
                &aggarwal_parallel_solver_on_neutral},
    .solver_num = 4,
};

int main() {
  unsigned int seed = (unsigned int)time(NULL);
  srand(seed);
  printf("Seed set to %u\n", seed);

  struct timespec start, finish;
  struct Interval *interval = interval_factory.generate_interval_by_imbalance(
      config.size, config.size * config.imbalance_percentage / 100);

  for (int solver_index = 0; solver_index < config.solver_num; solver_index++) {
    clock_gettime(CLOCK_MONOTONIC, &start);
    struct Mapping *mapping = config.solvers[solver_index]->solve(interval);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    mapping_free(mapping);
    printf("%s: %lf sec\n", config.solvers[solver_index]->name,
           (finish.tv_sec - start.tv_sec) +
               (finish.tv_nsec - start.tv_nsec) / 1000000000.0);
  }

  interval_free(interval);
  return 0;
}
