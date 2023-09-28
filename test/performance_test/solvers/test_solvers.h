#pragma once

#include "../../../src/solvers/solver.h"
#include "./performance.h"
#include "./test_cases.h"

struct Performance *
test_solvers_performance(struct PerformanceTestCases *test_cases);
