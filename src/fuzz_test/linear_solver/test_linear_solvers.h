#pragma once

#include <stdbool.h>

#include "../../lib/interval/interval.h"
#include "../../lib/linear_solver/linear_solver.h"
#include "../../lib/utils/range.h"

struct LinearSolversFuzzTestConfig {
  struct Interval *(*interval_generator)(int length, int imbalance);
  struct Range length_range;
  const struct LinearSolver **linear_solvers;
  int linear_solvers_num;
  double time_limit_in_seconds;
};

bool fuzz_test_linear_solvers(const struct LinearSolversFuzzTestConfig config);
