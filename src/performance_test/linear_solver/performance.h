#pragma once

#include "../../lib/linear_solver/linear_solver.h"

struct Performance {
  const struct LinearSolver *linear_solver;
  int interval_length;
  double imbalance_percentage;
  double time_taken;
};

struct PerformanceArray {
  struct Performance *performances;
  int length;
};

void performance_write_to_csv(const struct PerformanceArray *performance,
                              const char *file_name);

void performance_array_free(struct PerformanceArray *performance_array);
