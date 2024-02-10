#pragma once

#include "./performance.h"

struct PerformanceTestCasesConfig {
  const struct GridSize *grid_sizes;
  const double *imbalance_percentages;
  const struct GridSolver **grid_solvers;
  struct Grid *(*grid_generator)(struct GridSize size, double imbalance);
  int grid_sizes_num;
  int imbalance_percentages_num;
  int grid_solvers_num;
  int repetition_num;
};

struct PerformanceArray *
test_grid_solvers_performance(const struct PerformanceTestCasesConfig *config);
