#include <stdio.h>
#include <stdlib.h>

#include "./lib/grid/grid.h"
#include "./lib/grid_solver/grid_solver.h"
#include "./lib/utils/seed.h"
#include "./lib/utils/timer.h"
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

int main() {
  seed_set(0);
  int exit_status = EXIT_SUCCESS;

  struct Timer timer;
  timer_start(&timer);

  for (int i = 0; i < 5; i++) {
    struct Grid *grid =
        grid_factory.generate_compact_target_region_by_imbalance(
            config.width, config.height, config.target_region_size, i);

    struct Reconfiguration *reconfiguration =
        red_rec_parallel_multiple_consumers(
            grid, &(RedRecParallelParams){
                      .linear_solver =
                          &(struct LinearSolver){
                              .solve = linear_solve_aggarwal,
                              .params = NULL,
                              .name = "Aggarwal linear solver",
                          },
                      .thread_num = 1,
                  });

    reconfiguration_free(reconfiguration);
    grid_free(grid);
  }

  timer_stop(&timer);

  printf("Elapsed time: %f seconds\n", timer_get_seconds(&timer));
  return exit_status;
}
