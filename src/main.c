#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "./lib/grid/grid.h"
#include "./lib/interval/interval.h"
#include "./lib/interval/mapping.h"
#include "./lib/linear_solver/linear_solver.h"
#include "./lib/red-rec/red-rec.h"
#include "lib/point/point.h"
#include "lib/red-rec/reconfiguration.h"

int main() {
  // unsigned int seed = 1704741395u;
  unsigned int seed = (unsigned int)time(NULL);
  srand(seed);
  printf("Seed set to %u\n", seed);

  for (int i = 0; i < 10000; i++) {
    struct Grid *grid = grid_new_square_compact(21, 11);
    printf("Initial grid:\n");
    grid_print(grid);

    struct Reconfiguration *reconfiguration = red_rec(grid);

    if (reconfiguration != NULL) {
      reconfiguration_filter_identical(reconfiguration);
      reconfiguration_apply(reconfiguration, grid);
      reconfiguration_free(reconfiguration);
      printf("Final grid:\n");
      grid_print(grid);
      if (!grid_is_solved(grid)) {
        printf("Grid not solved!\n");
        exit(1);
      }
    }

    grid_free(grid);
  }
  return 0;
}
