#pragma once

struct PerformanceTestCasesConfig {
  const int *const interval_sizes;
  const int sizes_num;
  const double *const imbalance_percentages;
  const int imbalance_percentages_num;
  const int repetitions_per_test_case;
  struct Interval *(*const interval_generator)(const int size,
                                               const int imbalance);
};

struct PerformanceTestCases {
  struct Interval **const intervals;
  const int intervals_num;
};

struct PerformanceTestCases
generate_performance_tests(const struct PerformanceTestCasesConfig config);

void performance_test_cases_free(struct PerformanceTestCases test_cases);