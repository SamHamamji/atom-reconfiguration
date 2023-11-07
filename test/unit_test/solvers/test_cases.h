#pragma once

#include "../../../src/interval/interval.h"
#include "../../../src/mapping/mapping.h"

struct SolverTestCase {
  const struct Interval input;
  const struct Mapping expected_output;
};

extern const struct SolverTestCase solver_test_cases[];
extern const int solver_test_cases_num;
