#include <stdio.h>
#include <stdlib.h>

#include "../lib/solvers/solvers.h"
#include "./solvers/test_solvers.h"

int main(void) {
  int failed_tests = 0;
  for (int i = 0; i < solvers_num; i++) {
    if (!test_solver(*solvers[i])) {
      failed_tests++;
    }
  }

  if (failed_tests == 0) {
    printf("🟢 All tests passed\n");
  } else {
    printf("🔴 %d/%d tests failed\n", failed_tests, solvers_num);
    exit(EXIT_FAILURE);
  }
  return 0;
}
