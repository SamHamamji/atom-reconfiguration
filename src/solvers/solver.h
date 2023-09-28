#pragma once

#include "../interval/interval.h"
#include "../mapping/mapping.h"

typedef struct Mapping *SolverFunction(const struct Interval *interval);

struct Solver {
  const char *const name;
  SolverFunction *const solve;
};
