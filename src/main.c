#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "./lib/grid/grid.h"
#include "./lib/interval/interval.h"
#include "./lib/interval/mapping.h"
#include "./lib/linear_solver/linear_solver.h"
#include "./lib/red-rec/red-rec.h"

// struct Config {
//   int interval_length;
//   double imbalance_percentage;
//   const struct LinearSolver **linear_solvers;
//   int linear_solver_num;
// };

// static const struct LinearSolver *linear_solvers[] = {
//     &(struct LinearSolver){
//         .solve = linear_solve_aggarwal,
//         .params = NULL,
//         .name = "Aggarwal solver",
//     },
//     &(struct LinearSolver){
//         .solve = linear_solve_aggarwal_parallel,
//         .params = &(AggarwalParallelParams){.thread_num = 6},
//         .name = "Aggarwal solver parallel (6 threads)",
//     },
//     &(struct LinearSolver){
//         .solve = linear_solve_aggarwal_parallel,
//         .params = &(AggarwalParallelOnChainsParams){.thread_num = 6},
//         .name = "Aggarwal solver parallel on chains (6 threads)",
//     },
//     &(struct LinearSolver){
//         .solve = linear_solve_aggarwal_parallel,
//         .params = &(AggarwalParallelOnNeutralParams){.thread_num = 6},
//         .name = "Aggarwal solver parallel on neutral (6 threads)",
//     },
// };

// static const struct Config config = {
//     .interval_length = 50000000,
//     .imbalance_percentage = 10.0,
//     .linear_solvers = linear_solvers,
//     .linear_solver_num = sizeof(linear_solvers) / sizeof(linear_solvers[0]),
// };

// int main() {
//   unsigned int seed = (unsigned int)time(NULL);
//   srand(seed);
//   printf("Seed set to %u\n", seed);
//   struct timespec start, finish;
//   struct Interval *interval =
//   interval_factory.generate_interval_by_imbalance(
//       config.interval_length,
//       config.interval_length * config.imbalance_percentage / 100.0);
//   printf("Interval length: %d\nImbalance_percentage: %f\n",
//          config.interval_length, config.imbalance_percentage);
//   for (int i = 0; i < config.linear_solver_num; i++) {
//     const struct LinearSolver *linear_solver = config.linear_solvers[i];
//     clock_gettime(CLOCK_MONOTONIC, &start);
//     struct Mapping *mapping =
//         linear_solver->solve(interval, linear_solver->params);
//     clock_gettime(CLOCK_MONOTONIC, &finish);
//     mapping_free(mapping);
//     printf("%s: %lf sec\n", linear_solver->name,
//            (finish.tv_sec - start.tv_sec) +
//                (finish.tv_nsec - start.tv_nsec) / 1000000000.0);
//   }

//   interval_free(interval);
//   return 0;
// }

int main() {
  unsigned int seed = (unsigned int)time(NULL);
  srand(seed);
  printf("Seed set to %u\n", seed);

  struct Grid *grid = grid_new_square_compact(12, 12);
  printf("Grid size: %dx%d\n", grid->width, grid->height);
  printf("Initial grid:\n");
  grid_print(grid);

  struct Reconfiguration *reconfiguration = red_rec(grid);
  reconfiguration_apply(reconfiguration, grid);
  reconfiguration_free(reconfiguration);

  printf("Final grid:\n");
  grid_print(grid);

  grid_free(grid);
  return 0;
}
