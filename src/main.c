#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "./lib/interval/interval.h"
#include "./lib/mapping/mapping.h"
#include "./lib/solver/solver.h"

struct Config {
  int interval_length;
  double imbalance_percentage;
  const struct Solver **solvers;
  const struct Solver **params;
  int solver_num;
};

static const struct Solver *solvers[] = {
    &(struct Solver){
        .solve = aggarwal_solver_function,
        .params = NULL,
        .name = "Aggarwal solver",
    },
    &(struct Solver){
        .solve = aggarwal_parallel_solver_function,
        .params = &(AggarwalParallelParams){.thread_num = 4},
        .name = "Aggarwal solver parallel (4 threads)",
    },
    &(struct Solver){
        .solve = aggarwal_parallel_on_chains_solver_function,
        .params = &(AggarwalParallelOnChainsParams){.thread_num = 4},
        .name = "Aggarwal solver parallel on chains (4 threads)",
    },
    &(struct Solver){
        .solve = aggarwal_parallel_on_neutral_solver_function,
        .params = &(AggarwalParallelOnNeutralParams){.thread_num = 4},
        .name = "Aggarwal solver parallel on neutral (4 threads)",
    },
};

static const struct Config config = {
    .interval_length = 50000000,
    .imbalance_percentage = 10.0,
    .solvers = solvers,
    .solver_num = sizeof(solvers) / sizeof(solvers[0]),
};

int main() {
  unsigned int seed = (unsigned int)time(NULL);
  srand(seed);
  printf("Seed set to %u\n", seed);
  printf("Interval length: %d\nImbalance_percentage: %f\n",
         config.interval_length, config.imbalance_percentage);

  struct timespec start, finish;
  struct Interval *interval = interval_factory.generate_interval_by_imbalance(
      config.interval_length,
      config.interval_length * config.imbalance_percentage / 100.0);

  for (int solver_index = 0; solver_index < config.solver_num; solver_index++) {
    const struct Solver *solver = config.solvers[solver_index];
    clock_gettime(CLOCK_MONOTONIC, &start);
    struct Mapping *mapping = solver->solve(interval, solver->params);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    mapping_free(mapping);
    printf("%s: %lf sec\n", solver->name,
           (finish.tv_sec - start.tv_sec) +
               (finish.tv_nsec - start.tv_nsec) / 1000000000.0);
  }

  interval_free(interval);
  return 0;
}
