#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lib/utils/seed.h"
#include "./config.h"

int main(void) {
  seed_set_to_time();

  if (!fuzz_test_linear_solvers(linear_solvers_config)) {
    return EXIT_FAILURE;
  }
  printf("\n");

  seed_set_to_time();
  if (!fuzz_test_grid_solvers(grid_solvers_config)) {
    return EXIT_FAILURE;
  }
  printf("\n");

  printf("🟢 All tests passed.\n");
}
