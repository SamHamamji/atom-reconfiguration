#include <stdlib.h>
#include <time.h>

#include "../../lib/solver/solver.h"
#include "./test_solvers.h"
#include "performance.h"
#include "test_cases.h"

static double test_solver_performance(const struct Solver *solver,
                                      const struct Interval *interval) {
  struct timespec start, finish;
  clock_gettime(CLOCK_MONOTONIC, &start);
  struct Mapping *mapping = solver->solve(interval, solver->params);
  clock_gettime(CLOCK_MONOTONIC, &finish);
  mapping_free(mapping);

  return (finish.tv_sec - start.tv_sec) +
         (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
}

struct PerformanceArray *
test_solvers_performance(const struct PerformanceTestCasesConfig *config) {
  struct PerformanceTestCases *test_cases = generate_performance_tests(config);

  struct PerformanceArray *performance_array =
      malloc(sizeof(struct PerformanceArray));
  performance_array->length = test_cases->intervals_num * config->solvers_num;
  performance_array->performances =
      calloc(performance_array->length, sizeof(struct Performance));

  for (int solver_index = 0; solver_index < config->solvers_num;
       solver_index++) {
    for (int i = 0; i < test_cases->intervals_num; i++) {
      double time_taken = test_solver_performance(config->solvers[solver_index],
                                                  test_cases->intervals[i]);

      performance_array->performances[i * config->solvers_num + solver_index] =
          (struct Performance){
              .solver = config->solvers[solver_index],
              .interval_size = test_cases->intervals[i]->size,
              .imbalance_percentage = test_cases->imbalance_percentages[i],
              .time_taken = time_taken,
          };
    }
  }

  performance_test_cases_free(test_cases);
  return performance_array;
}
