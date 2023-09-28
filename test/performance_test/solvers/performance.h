#include <stdio.h>

#include "../../../src/solvers/solver.h"

struct Performance {
  const struct Solver *solver;
  int interval_size;
  int imbalance;
  double time_taken;
};

void performance_write_to_csv(const struct Performance *const performance,
                              const int performance_count, FILE *const file);
