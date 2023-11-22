#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../../src/solvers/solver.h"
#include "../../../src/solvers/solvers.h"
#include "./test_solvers.h"

struct Performance *
test_solvers_performance(const struct PerformanceTestCases *const test_cases) {
  struct Performance *const performances = calloc(
      test_cases->intervals_num * solvers_num, sizeof(struct Performance));

  for (int solver_index = 0; solver_index < solvers_num; solver_index++) {
    for (int i = 0; i < test_cases->intervals_num; i++) {
      struct timespec start;
      struct timespec finish;
      clock_gettime(CLOCK_MONOTONIC, &start);
      struct Mapping *const mapping =
          solvers[solver_index]->solve(test_cases->intervals[i]);
      clock_gettime(CLOCK_MONOTONIC, &finish);

      mapping_free(mapping);

      performances[i * solvers_num + solver_index] = (struct Performance){
          .solver = solvers[solver_index],
          .interval_size = test_cases->intervals[i]->size,
          .imbalance = interval_get_imbalance(test_cases->intervals[i]),
          .time_taken = (finish.tv_sec - start.tv_sec) +
                        (finish.tv_nsec - start.tv_nsec) / 1000000000.0,
      };
    }
  }
  return performances;
}
