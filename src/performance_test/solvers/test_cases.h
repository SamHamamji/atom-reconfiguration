#pragma once

#include "../../lib/solver/solver.h"

struct PerformanceTestCasesConfig {
  const int *interval_sizes;
  const double *imbalance_percentages;
  const struct Solver **solvers;
  struct Interval *(*interval_generator)(int size, int imbalance);
  const int sizes_num;
  const int imbalance_percentages_num;
  const int solvers_num;
  const int repetitions_per_test_case;
};

struct PerformanceTestCases {
  struct Interval **intervals;
  double *imbalance_percentages;
  int intervals_num;
};

struct PerformanceTestCases *
generate_performance_tests(const struct PerformanceTestCasesConfig *config);

void performance_test_cases_free(struct PerformanceTestCases *test_cases);
