#include <stdio.h>
#include <stdlib.h>

#include "../../lib/utils/seed.h"
#include "./config.h"
#include "./test_linear_solvers.h"

int main(void) {
  seed_set_to_time();

  if (!fuzz_test_linear_solvers(linear_solvers_config)) {
    return EXIT_FAILURE;
  }

  printf("\n");
  printf("🟢 All tests passed.\n");
}
