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
    .height = 1000,
    .width = 1000,
    .target_region_size = 500,
};

static const RedRecParallelParams params = {
    .linear_solver =
        &(struct LinearSolver){
            .solve = linear_solve_aggarwal,
            .params = NULL,
            .name = "",
        },
    .thread_num = 5,
};

int main() {
  seed_set_to_time();

  for (int i = 0; i < 1000; i++) {
    struct Grid *grid = grid_factory.generate_compact_target_region(
        config.width, config.height, config.target_region_size);

    struct Reconfiguration *reconfiguration =
        red_rec_deferred_solving(grid, &params);

    reconfiguration_free(reconfiguration);
    grid_free(grid);
  }

  int exit_status = EXIT_SUCCESS;
  return exit_status;
}
