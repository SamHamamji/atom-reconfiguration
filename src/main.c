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
    .height = 4000,
    .width = 4000,
    .target_region_size = 2000,
};

static const RedRecParallelParams params = {
    .linear_solver =
        &(struct LinearSolver){
            .solve = linear_solve_aggarwal,
            .params = NULL,
            .name = "",
        },
    .thread_num = 10,
};

int main() {
  seed_set_to_time();

  for (int i = 0; i < 10; i++) {
    struct Grid *grid = grid_factory.generate_compact_target_region(
        config.width, config.height, config.target_region_size);

    struct Reconfiguration *reconfiguration =
        red_rec_parallel_multiple_consumers(grid, &params);

    reconfiguration_free(reconfiguration);
    grid_free(grid);
  }

  int exit_status = EXIT_SUCCESS;
  return exit_status;
}
