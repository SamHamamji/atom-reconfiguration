#pragma once

#include "../../lib/solver/solver.h"
#include "./performance.h"

struct PerformanceTestCasesConfig {
  const int *interval_lengths;
  const double *imbalance_percentages;
  const struct Solver **solvers;
  struct Interval *(*interval_generator)(int length, int imbalance);
  int lengths_num;
  int imbalance_percentages_num;
  int solvers_num;
  int repetition_num;
};

struct PerformanceArray *
test_solvers_performance(const struct PerformanceTestCasesConfig *config);
