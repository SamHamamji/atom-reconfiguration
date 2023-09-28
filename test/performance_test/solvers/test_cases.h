#pragma once

struct PerformanceTestCasesConfig {
  const int *const interval_sizes;
  const int sizes_num;
  const int tests_per_size;
};

struct PerformanceTestCases {
  struct Interval *const intervals;
  const int intervals_num;
};

struct PerformanceTestCases
generate_performance_tests(const struct PerformanceTestCasesConfig config);

void performance_test_cases_free(struct PerformanceTestCases test_cases);