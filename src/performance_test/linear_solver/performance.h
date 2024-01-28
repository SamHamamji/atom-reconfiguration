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

struct PerformanceArray *performance_array_new(int length);

void performance_array_free(struct PerformanceArray *performance_array);

void performance_write_to_csv(const struct PerformanceArray *performance,
                              const char *file_name);
