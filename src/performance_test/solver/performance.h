#pragma once

#include "../../lib/solver/solver.h"

struct Performance {
  const struct Solver *solver;
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