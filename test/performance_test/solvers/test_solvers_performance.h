#pragma once

#include "../../../src/solvers/solver.h"

struct Performance {
  const struct Solver *solver;
  int interval_size;
  int imbalance;
  double time_taken;
};

struct Performance *
test_solvers_performance(struct PerformanceTestCases *test_cases);
