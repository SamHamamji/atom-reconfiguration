#pragma once

#include "../../lib/solvers/solver.h"

struct PerformanceTestCasesConfig {
  const int *const interval_sizes;
  const double *const imbalance_percentages;
  const struct Solver *const *solvers;
  struct Interval *(*const interval_generator)(const int size,
                                               const int imbalance);
  const int sizes_num;
  const int imbalance_percentages_num;
  const int repetitions_per_test_case;
  const int solvers_num;
};

struct PerformanceTestCases {
  struct Interval **intervals;
  double *imbalance_percentages;
  int intervals_num;
};

struct PerformanceTestCases *
generate_performance_tests(const struct PerformanceTestCasesConfig *config);

void performance_test_cases_free(struct PerformanceTestCases *test_cases);