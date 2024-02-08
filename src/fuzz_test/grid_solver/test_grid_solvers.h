#pragma once

#include <stdbool.h>

#include "../../lib/utils/range.h"

struct GridSolversFuzzTestConfig {
  struct Grid *(*grid_generator)(int width, int height);
  struct Range height_range;
  struct Range width_range;
  const struct GridSolver **grid_solvers;
  int grid_solvers_num;
  double time_limit_in_seconds;
};

bool fuzz_test_grid_solvers(const struct GridSolversFuzzTestConfig config);
