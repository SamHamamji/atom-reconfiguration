#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "./lib/interval/interval.h"
#include "./lib/mapping/mapping.h"
#include "./lib/solver/solver.h"

struct Config {
  int interval_length;
  int imbalance;
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
        .params = &(AggarwalParallelOnChainsParams){.thread_num = 3},
        .name = "Aggarwal solver parallel (3 threads)",
    },
    &(struct Solver){
        .solve = aggarwal_parallel_on_chains_solver_function,
        .params = &(AggarwalParallelOnChainsParams){.thread_num = 3},
        .name = "Aggarwal solver parallel on chains (3 threads)",
    },
    &(struct Solver){
        .solve = aggarwal_parallel_on_neutral_solver_function,
        .params = &(AggarwalParallelOnNeutralParams){.thread_num = 3},
        .name = "Aggarwal solver parallel on neutral (3 threads)",
    },
};

static const struct Config config = {
    .interval_length = 1000,
    .imbalance = 10,
    .solvers = solvers,
    .solver_num = sizeof(solvers) / sizeof(solvers[0]),
};

int main() {
  unsigned int seed = (unsigned int)time(NULL);
  srand(seed);
  printf("Seed set to %u\n", seed);
  printf("Interval length: %d\nImbalance: %d\n", config.interval_length,
         config.imbalance);

  struct timespec start, finish;
  struct Interval *interval = interval_factory.generate_interval_by_imbalance(
      config.interval_length, config.imbalance);

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
