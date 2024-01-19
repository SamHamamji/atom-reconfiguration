#include <stdio.h>

#include "../../lib/linear_solver/linear_solver.h"
#include "../../lib/utils/colors.h"
#include "./test_cases.h"
#include "./test_linear_solvers.h"

static void print_failed_test_case(const struct LinearSolverTestCase test_case,
                                   const int test_case_index,
                                   const struct Mapping *result) {
  printf("Failed test case %d\n", test_case_index);
  printf("Interval:\n");
  interval_print(test_case.input);
  printf("Expected:\n");
  mapping_print(test_case.expected_output);
  printf("Received:\n");
  mapping_print(result);
}

static bool test_linear_solver(const struct LinearSolver *linear_solver) {
  int failed_test_cases = 0;

  for (int i = 0; i < linear_solver_test_cases_num; i++) {
    struct Mapping *mapping = linear_solver->solve(
        linear_solver_test_cases[i]->input, linear_solver->params);
    bool mappings_are_equal =
        mapping_equals(mapping, linear_solver_test_cases[i]->expected_output);
    if (!mappings_are_equal) {
      if (failed_test_cases == 0) {
        print_failed_test_case(*linear_solver_test_cases[i], i + 1, mapping);
      }
      failed_test_cases++;
    }

    mapping_free(mapping);
  }

  if (failed_test_cases != 0) {
    printf("Failed %d/%d test cases\n\n", failed_test_cases,
           linear_solver_test_cases_num);
  }

  return failed_test_cases == 0;
}

bool test_linear_solvers(const struct LinearSolver *linear_solvers[],
                         int linear_solver_num) {
  printf(BOLD "Testing linear solvers:\n" RESET);
  int failed_tests = 0;
  for (int i = 0; i < linear_solver_num; i++) {
    printf("%s...\n", linear_solvers[i]->name);
    if (!test_linear_solver(linear_solvers[i])) {
      failed_tests++;
    }
  }
  if (failed_tests == 0) {
    printf("🟢 All tests passed.\n");
  } else {
    printf("🔴 %d/%d tests failed!\n", failed_tests, linear_solver_num);
  }
  return failed_tests == 0;
}
