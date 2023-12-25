#pragma once

#include "../../lib/interval/interval.h"
#include "../../lib/mapping/mapping.h"

struct SolverTestCase {
  const struct Interval input;
  const struct Mapping expected_output;
};

extern const struct SolverTestCase solver_test_cases[];
extern const int solver_test_cases_num;
