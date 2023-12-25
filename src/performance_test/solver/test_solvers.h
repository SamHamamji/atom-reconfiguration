#pragma once

#include "../../lib/solver/solver.h"
#include "./performance.h"
#include "./test_cases.h"

struct PerformanceArray *
test_solvers_performance(const struct PerformanceTestCasesConfig *config);
