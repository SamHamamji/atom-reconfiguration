#pragma once

#include "../../lib/interval/interval.h"
#include "../../lib/interval/mapping.h"

struct LinearSolverTestCase {
  struct Interval *input;
  const struct Mapping *expected_output;
};

extern const struct LinearSolverTestCase *linear_solver_test_cases[];
extern const int linear_solver_test_cases_num;
