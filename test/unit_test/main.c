#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../src/solvers/solvers.h"
#include "../../unity/unity.h"
#include "./solvers/test_solvers.h"

void test_iterative_solver(void) { test_solver(iterative_solver); }
void test_karp_li_solver(void) { test_solver(karp_li_solver); }

static void (*const unit_tests[])(void) = {
    test_iterative_solver,
    test_karp_li_solver,
};
static const int unit_tests_num = sizeof(unit_tests) / sizeof(unit_tests[0]);

void setUp(void) {}

void tearDown(void) {}

int main(void) {
  UNITY_BEGIN();
  for (int i = 0; i < unit_tests_num; i++) {
    printf("\n");
    RUN_TEST(unit_tests[i]);
  }
  return UNITY_END();
}
