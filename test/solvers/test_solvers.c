#include <stdio.h>
#include <string.h>

#include "../../src/solvers/solvers.h"
#include "../../unity/unity.h"
#include "./test_cases.h"
#include "./test_solvers.h"

extern const struct SolverTestCase solver_test_cases[];
extern const int solver_test_cases_num;

void test_solver(Solver solver) {
  for (int i = 0; i < solver_test_cases_num; i++) {
    struct Mapping *mapping = solver(&solver_test_cases[i].input);
    int mappings_are_equal =
        mapping_equals(mapping, &solver_test_cases[i].expected_output);
    if (mappings_are_equal) {
      printf("Passed subtest %d\n", i + 1);
    } else {
      printf("Failed subtest %d\n", i + 1);
      printf("Expected:\n");
      mapping_print(&solver_test_cases[i].expected_output);
      printf("Got:\n");
      mapping_print(mapping);
    }
    TEST_ASSERT(mappings_are_equal);
  }
}

void test_iterative_solver(void) { test_solver(iterative_solver); }
void test_karp_li_solver(void) { test_solver(karp_li_solver); }
