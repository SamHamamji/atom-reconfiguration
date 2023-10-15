#include <stdio.h>
#include <stdlib.h>

#include "../../../src/solvers/solvers.h"
#include "test_cases.h"

struct PerformanceTestCases
generate_performance_tests(const struct PerformanceTestCasesConfig config) {
  int intervals_num = config.sizes_num * config.tests_per_size;
  struct Interval **intervals = calloc(intervals_num, sizeof(struct Interval));

  for (int i = 0; i < config.sizes_num; i++) {
    for (int j = 0; j < config.tests_per_size; j++) {
      int target_num = rand() % (config.interval_sizes[i] / 2);
      int imbalance = rand() % (config.interval_sizes[i] - 2 * target_num);
      intervals[i * config.tests_per_size + j] =
          interval_factory.generate_interval(
              config.interval_sizes[i], target_num, target_num + imbalance);
    }
  }
  struct PerformanceTestCases test_cases = {
      .intervals = intervals,
      .intervals_num = intervals_num,
  };
  return test_cases;
}

void performance_test_cases_free(struct PerformanceTestCases test_cases) {
  for (int i = 0; i < test_cases.intervals_num; i++) {
    interval_free(test_cases.intervals[i]);
  }
  free(test_cases.intervals);
}
