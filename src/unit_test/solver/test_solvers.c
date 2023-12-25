#include <stdio.h>

#include "../../lib/solver/solver.h"
#include "./test_cases.h"
#include "./test_solvers.h"

static void print_failed_test_case(const struct SolverTestCase test_case,
                                   const int test_case_index,
                                   const struct Mapping *result) {
  printf("Failed test case %d\n", test_case_index);
  printf("Interval:\n");
  interval_print(&test_case.input);
  printf("Expected:\n");
  mapping_print(&test_case.expected_output);
  printf("Received:\n");
  mapping_print(result);
}

static bool test_solver(const struct Solver *solver) {
  int failed_test_cases = 0;

  for (int i = 0; i < solver_test_cases_num; i++) {
    struct Mapping *mapping =
        solver->solve(&solver_test_cases[i].input, solver->params);
    bool mappings_are_equal =
        mapping_equals(mapping, &solver_test_cases[i].expected_output);
    if (!mappings_are_equal) {
      if (failed_test_cases == 0) {
        print_failed_test_case(solver_test_cases[i], i + 1, mapping);
      }
      failed_test_cases++;
    }

    mapping_free(mapping);
  }

  if (failed_test_cases != 0) {
    printf("Failed %d/%d test cases\n\n", failed_test_cases,
           solver_test_cases_num);
  }

  return failed_test_cases == 0;
}

bool test_solvers(const struct Solver *solvers[], int solver_num) {
  int failed_tests = 0;
  for (int i = 0; i < solver_num; i++) {
    printf("Testing %s...\n", solvers[i]->name);
    if (!test_solver(solvers[i])) {
      failed_tests++;
    }
  }
  if (failed_tests == 0) {
    printf("🟢 All tests passed\n");
  } else {
    printf("🔴 %d/%d tests failed\n", failed_tests, solver_num);
  }
  return failed_tests == 0;
}
