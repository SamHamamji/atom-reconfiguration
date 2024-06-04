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
  int imbalance;
} config = {
    .height = 10,
    .width = 10,
    .target_region_size = 5,
    .imbalance = 3,
};

int main() {
  seed_set(0);

  struct Timer timer;
  timer_start(&timer);

  struct Grid *grid = grid_factory.generate_compact_target_region_by_imbalance(
      config.width, config.height, config.target_region_size, config.imbalance);

  struct Reconfiguration *reconfiguration =
      red_rec(grid, &(RedRecParallelParams){
                        .linear_solver =
                            &(struct LinearSolver){
                                .solve = linear_solve_aggarwal,
                                .params = NULL,
                                .name = "Aggarwal linear solver",
                            },
                        .thread_num = 1,
                        .pq_type = HEAP_PRIORITY_QUEUE,
                    });

  grid_print(grid);
  reconfiguration_print(reconfiguration);

  reconfiguration_free(reconfiguration);
  grid_free(grid);

  timer_stop(&timer);

  printf("Elapsed time: %f seconds\n", timer_get_seconds(&timer));
  return EXIT_SUCCESS;
}
