#pragma once

#include "../../lib/linear_solver/linear_solver.h"
#include "./performance.h"

struct PerformanceTestCasesConfig {
  const int *interval_lengths;
  const double *imbalance_percentages;
  const struct LinearSolver **linear_solvers;
  struct Interval *(*interval_generator)(int length, int imbalance);
  int lengths_num;
  int imbalance_percentages_num;
  int linear_solvers_num;
  int repetition_num;
};

struct PerformanceArray *test_linear_solvers_performance(
    const struct PerformanceTestCasesConfig *config);
