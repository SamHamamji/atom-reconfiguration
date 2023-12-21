#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int test_solver(struct Solver solver) {
  printf("Testing %s...\n", solver.name);
  int failed_tests_num = 0;

  for (int i = 0; i < solver_test_cases_num; i++) {
    // printf("TESTCASE %d\n", i + 1);
    struct Mapping *mapping = solver.solve(&solver_test_cases[i].input);
    bool mappings_are_equal =
        mapping_equals(mapping, &solver_test_cases[i].expected_output);
    if (!mappings_are_equal) {
      if (failed_tests_num == 0) {
        print_failed_test_case(solver_test_cases[i], i + 1, mapping);
      }
      failed_tests_num++;
    }

    mapping_free(mapping);
  }

  if (failed_tests_num != 0) {
    printf("Failed %d/%d test cases\n\n", failed_tests_num,
           solver_test_cases_num);
  }

  return failed_tests_num == 0;
}
