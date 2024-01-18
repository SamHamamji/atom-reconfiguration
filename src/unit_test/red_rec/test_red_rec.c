#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../lib/grid/grid.h"
#include "../../lib/red-rec/reconfiguration.h"
#include "../../lib/red-rec/red-rec.h"
#include "./test_red_rec.h"

int get_random_int(int min, int max) { return min + rand() % (max - min + 1); }

bool test_red_rec(const struct RedRecUnitTestConfig *config) {
  for (int i = 0; i < config->test_case_num; i++) {
    struct Grid *grid = grid_factory.generate_compact_target_region(
        get_random_int(config->min_width, config->max_width),
        get_random_int(config->min_height, config->max_height));
    struct Reconfiguration *reconfiguration = red_rec(grid);

    if (reconfiguration != NULL) {
      // printf("Initial grid:\n");
      // grid_print(grid);
      reconfiguration_filter_identical(reconfiguration);
      reconfiguration_apply(reconfiguration, grid);
      // printf("Final grid:\n");
      // grid_print(grid);
      if (!grid_is_solved(grid)) {
        printf("Grid not solved!\n");
        exit(1);
      }
    } else {
      printf("No solution found!\n");
    }

    grid_free(grid);
    reconfiguration_free(reconfiguration);
  }
  return true;
}
