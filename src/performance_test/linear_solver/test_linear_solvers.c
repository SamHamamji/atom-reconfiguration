#include "./test_linear_solvers.h"
#include "../../lib/linear_solver/linear_solver.h"
#include "../../lib/utils/timer.h"
#include "performance.h"

static double test_linear_solver_performance_on_interval(
    const struct LinearSolver *linear_solver, const struct Interval *interval) {
  struct Timer timer;

  timer_start(&timer);
  struct Mapping *mapping =
      linear_solver->solve(interval, linear_solver->params);
  timer_stop(&timer);

  mapping_free(mapping);

  return timer_get_seconds(&timer);
}

struct PerformanceArray *test_linear_solvers_performance(
    const struct PerformanceTestCasesConfig *config) {
  struct PerformanceArray *performance_array = performance_array_new(
      config->linear_solvers_num * config->lengths_num *
      config->imbalance_percentages_num * config->repetition_num);

  int test_case_index = 0;
  for (int i = 0; i < config->lengths_num; i++) {
    for (int j = 0; j < config->imbalance_percentages_num; j++) {
      for (int k = 0; k < config->repetition_num; k++) {
        struct Interval *interval = config->interval_generator(
            config->interval_lengths[i], config->imbalance_percentages[j]);
        for (int linear_solver_index = 0;
             linear_solver_index < config->linear_solvers_num;
             linear_solver_index++) {
          double time_taken = test_linear_solver_performance_on_interval(
              config->linear_solvers[linear_solver_index], interval);

          performance_array->performances[test_case_index] =
              (struct Performance){
                  .linear_solver = config->linear_solvers[linear_solver_index],
                  .interval_length = interval->length,
                  .imbalance_percentage = config->imbalance_percentages[j],
                  .time_taken = time_taken,
              };
          test_case_index++;
        }
        interval_free(interval);
      }
    }
  }
  return performance_array;
}
