#pragma once

#include "../../lib/grid/grid.h"
#include "../../lib/grid_solver/grid_solver.h"
#include "../../lib/utils/range.h"

struct GridSolversFuzzTestConfig {
  struct Grid *(*grid_generator)(int width, int height);
  struct Range height_range;
  struct Range width_range;
  const struct GridSolver **grid_solvers;
  int grid_solvers_num;
  double time_limit_in_seconds;
};

extern struct GridSolversFuzzTestConfig grid_solvers_config;
