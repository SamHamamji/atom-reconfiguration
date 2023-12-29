#include <stdlib.h>
#include <time.h>

#include "../../lib/solver/solver.h"
#include "./test_solvers.h"
#include "performance.h"

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
  struct PerformanceArray *performance_array =
      malloc(sizeof(struct PerformanceArray));
  performance_array->length = config->solvers_num * config->lengths_num *
                              config->imbalance_percentages_num *
                              config->repetition_num;
  performance_array->performances =
      malloc(performance_array->length * sizeof(struct Performance));

  int testcase_index = 0;
  for (int i = 0; i < config->lengths_num; i++) {
    for (int j = 0; j < config->imbalance_percentages_num; j++) {
      for (int k = 0; k < config->repetition_num; k++) {
        int imbalance = (int)(config->interval_lengths[i] *
                              config->imbalance_percentages[j] / 100);
        struct Interval *interval =
            config->interval_generator(config->interval_lengths[i], imbalance);
        for (int solver_index = 0; solver_index < config->solvers_num;
             solver_index++) {
          double time_taken =
              test_solver_performance(config->solvers[solver_index], interval);

          performance_array->performances[testcase_index] =
              (struct Performance){
                  .solver = config->solvers[solver_index],
                  .interval_length = interval->length,
                  .imbalance_percentage = config->imbalance_percentages[j],
                  .time_taken = time_taken,
              };
          testcase_index++;
        }
        interval_free(interval);
      }
    }
  }
  return performance_array;
}
