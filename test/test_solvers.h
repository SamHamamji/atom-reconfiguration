#pragma once

#include <stdlib.h>

#include "../src/interval/interval.h"
#include "../src/mapping/mapping.h"
#include "../src/solvers/solvers.h"

struct SolverTestCase {
  const struct Interval input;
  const struct Mapping expected_output;
};

void test_iterative_solver(void);
void test_karp_li_solver(void);
