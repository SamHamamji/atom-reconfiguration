#pragma once

#include "../../lib/grid_solver/grid_solver.h"

struct GridSize {
  int width;
  int height;
};

struct Performance {
  const struct GridSolver *grid_solver;
  struct GridSize grid_size;
  double imbalance_percentage;
  double time_taken;
};

struct PerformanceArray {
  struct Performance *performances;
  int length;
};

struct PerformanceArray *performance_array_new(int length);

void performance_array_free(struct PerformanceArray *performance_array);

void performance_write_to_csv(const struct PerformanceArray *performance,
                              const char *file_name);
