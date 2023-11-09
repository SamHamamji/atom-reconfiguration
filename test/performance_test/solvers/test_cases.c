#include <stdlib.h>

#include "../../../src/solvers/solvers.h"
#include "test_cases.h"

struct PerformanceTestCases
generate_performance_tests(const struct PerformanceTestCasesConfig config) {
  int intervals_num = config.sizes_num * config.imbalance_percentages_num *
                      config.repetitions_per_test_case;
  struct Interval **intervals = calloc(intervals_num, sizeof(struct Interval));

  for (int i = 0; i < config.sizes_num; i++) {
    for (int j = 0; j < config.imbalance_percentages_num; j++) {
      for (int k = 0; k < config.repetitions_per_test_case; k++) {
        const int imbalance = (int)(config.interval_sizes[i] *
                                    config.imbalance_percentages[j] / 100);
        intervals[k + config.repetitions_per_test_case *
                          (i * config.imbalance_percentages_num + j)] =
            config.interval_generator(config.interval_sizes[i], imbalance);
      }
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
