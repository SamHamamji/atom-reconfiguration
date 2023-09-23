#include <stdlib.h>

#include "../unity/unity.h"
#include "./solvers/test_solvers.h"

void setUp(void) {}

void tearDown(void) {}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_iterative_solver);
  RUN_TEST(test_karp_li_solver);
  return UNITY_END();
}
