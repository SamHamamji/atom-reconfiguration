#include <stdio.h>
#include <stdlib.h>

#include "./lib/grid/grid.h"
#include "./lib/grid_solver/grid_solver.h"
#include "./lib/utils/seed.h"
#include "lib/grid/reconfiguration.h"
#include "lib/linear_solver/linear_solver.h"

static const struct Config {
  int height;
  int width;
  int target_region_size;
} config = {
    .height = 2000,
    .width = 2000,
    .target_region_size = 1000,
};

static const RedRecParams params = {
    .linear_solver =
        &(struct LinearSolver){
            .solve = linear_solve_aggarwal,
            .params = NULL,
            .name = "",
        },
};

int main() {
  seed_set_to_time();

  struct Grid *grid = grid_factory.generate_compact_target_region(
      config.width, config.height, config.target_region_size);
  printf("Initial grid:\n");
  grid_print(grid);

  struct Reconfiguration *reconfiguration = red_rec(grid, &params);

  grid_apply_reconfiguration(grid, reconfiguration);
  reconfiguration_free(reconfiguration);

  int exit_status = EXIT_SUCCESS;

  if (reconfiguration == NULL) {
    printf("No solution available.\n");
  } else if (grid_is_solved(grid)) {
    printf("Final grid:\n");
    grid_print(grid);
    printf("Grid solved successfully!\n");
  } else {
    printf("Grid not solved!\n");
    exit_status = EXIT_FAILURE;
  }

  grid_free(grid);

  return exit_status;
}
