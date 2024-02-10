#include <stdio.h>
#include <stdlib.h>

#include "./lib/grid/grid.h"
#include "./lib/grid_solver/grid_solver.h"
#include "./lib/utils/seed.h"
#include "lib/grid/reconfiguration.h"
#include "lib/linear_solver/linear_solver.h"
// #include "lib/point/point.h"

// static struct Grid *grid_1 = &(struct Grid){
//     (struct Point[]){
//         EMPTY,  SOURCE, BOTH,   BOTH,   BOTH,   TARGET, BOTH,   BOTH, EMPTY,
//         SOURCE, EMPTY,  SOURCE, EMPTY,  TARGET, BOTH,   BOTH,   TARGET, BOTH,
//         BOTH,   EMPTY,  SOURCE, SOURCE, SOURCE, SOURCE, TARGET, TARGET, BOTH,
//         BOTH,   BOTH,   TARGET, SOURCE, SOURCE, SOURCE, SOURCE, EMPTY,
//         TARGET, BOTH,   BOTH,   BOTH,   BOTH,   BOTH,   EMPTY,  EMPTY,
//         SOURCE, EMPTY, SOURCE, TARGET, BOTH,   BOTH,   BOTH,   BOTH, TARGET,
//         SOURCE, SOURCE, SOURCE, SOURCE, SOURCE, BOTH,   TARGET, BOTH,   BOTH,
//         TARGET, TARGET, EMPTY,  SOURCE, SOURCE, EMPTY,  EMPTY,  TARGET, BOTH,
//         TARGET, BOTH, BOTH,   TARGET, SOURCE, EMPTY,  EMPTY,  SOURCE, SOURCE,
//         BOTH,   BOTH, TARGET, BOTH,   TARGET, BOTH,   SOURCE, SOURCE, SOURCE,
//         EMPTY,  SOURCE, TARGET, BOTH,   BOTH,   TARGET, BOTH,   TARGET,
//         SOURCE, SOURCE, SOURCE, EMPTY,  EMPTY,  BOTH,   BOTH,   TARGET, BOTH,
//         TARGET, TARGET, SOURCE, EMPTY,  SOURCE, EMPTY,  SOURCE, BOTH, TARGET,
//         BOTH,   BOTH,   BOTH, TARGET, EMPTY,  SOURCE, SOURCE, EMPTY,  EMPTY,
//         BOTH,   TARGET, BOTH, BOTH,   TARGET, TARGET, EMPTY,  EMPTY,  EMPTY,
//         SOURCE, SOURCE, TARGET, BOTH,   TARGET, TARGET, TARGET, TARGET,
//         SOURCE, SOURCE, SOURCE, SOURCE, SOURCE, TARGET, TARGET, TARGET,
//         TARGET, TARGET, BOTH,   SOURCE, EMPTY, SOURCE, EMPTY,  EMPTY, TARGET,
//         TARGET, TARGET, TARGET, TARGET, TARGET, EMPTY,  EMPTY,  EMPTY,
//         SOURCE, SOURCE, TARGET, BOTH,   BOTH,   TARGET, BOTH,   TARGET,
//         SOURCE, EMPTY,  SOURCE, SOURCE, EMPTY,  BOTH,   BOTH, TARGET, TARGET,
//         TARGET, TARGET, SOURCE, SOURCE, EMPTY,  SOURCE, SOURCE, BOTH,   BOTH,
//         BOTH,   BOTH,   TARGET, BOTH,   SOURCE, SOURCE, EMPTY, SOURCE, EMPTY,
//         TARGET, BOTH,   BOTH,   TARGET, TARGET, BOTH,   EMPTY, SOURCE,
//         SOURCE, SOURCE, EMPTY,  BOTH,   TARGET, BOTH,   BOTH,   BOTH, TARGET,
//         EMPTY,  SOURCE, SOURCE, SOURCE, EMPTY,  TARGET, TARGET, TARGET,
//         TARGET, TARGET, BOTH,   EMPTY,  SOURCE, EMPTY,
//     },
//     .width = 21,
//     .height = 11,
// };

// static struct Grid *grid_2 = &(struct Grid){
//     (struct Point[]){
//         EMPTY,  SOURCE, BOTH,   BOTH,   BOTH,   BOTH,   BOTH,   BOTH, EMPTY,
//         SOURCE, EMPTY,  EMPTY,  EMPTY,  BOTH,   BOTH,   BOTH,   BOTH,   BOTH,
//         BOTH,   EMPTY,  EMPTY,  SOURCE, SOURCE, SOURCE, BOTH,   BOTH,   BOTH,
//         BOTH,   BOTH,   BOTH,   EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  BOTH,
//         BOTH,   BOTH,   BOTH,   BOTH,   BOTH,   EMPTY,  EMPTY,  EMPTY, EMPTY,
//         EMPTY,  BOTH,   BOTH,   BOTH,   BOTH,   BOTH,   BOTH,   EMPTY, EMPTY,
//         EMPTY,  EMPTY,  EMPTY,  BOTH,   BOTH,   BOTH,   BOTH,   BOTH,   BOTH,
//         EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  BOTH,   BOTH,   BOTH,   BOTH,
//         BOTH,   BOTH,   EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  BOTH,   BOTH,
//         BOTH,   BOTH,   BOTH,   BOTH,   EMPTY,  EMPTY,  EMPTY,  EMPTY, EMPTY,
//         BOTH,   BOTH,   BOTH,   BOTH,   BOTH,   BOTH,   EMPTY,  EMPTY, EMPTY,
//         EMPTY,  EMPTY,  BOTH,   BOTH,   BOTH,   BOTH,   BOTH,   BOTH, EMPTY,
//         EMPTY,  EMPTY,  EMPTY,  EMPTY,  BOTH,   BOTH,   BOTH,   BOTH,   BOTH,
//         BOTH,   EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  BOTH,   BOTH,   BOTH,
//         BOTH,   BOTH,   BOTH,   EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  BOTH,
//         BOTH,   BOTH,   BOTH,   BOTH,   BOTH,   EMPTY,  EMPTY,  EMPTY, EMPTY,
//         EMPTY,  TARGET, TARGET, TARGET, TARGET, TARGET, TARGET, EMPTY, EMPTY,
//         EMPTY,  SOURCE, EMPTY,  BOTH,   BOTH,   BOTH,   BOTH,   BOTH,   BOTH,
//         SOURCE, EMPTY,  EMPTY,  EMPTY,  EMPTY,  BOTH,   BOTH,   BOTH,   BOTH,
//         BOTH,   BOTH,   EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  BOTH,   BOTH,
//         BOTH,   BOTH,   BOTH,   BOTH,   EMPTY,  EMPTY,  SOURCE, EMPTY, EMPTY,
//         BOTH,   BOTH,   BOTH,   BOTH,   BOTH,   BOTH,   EMPTY,  SOURCE,
//         EMPTY, EMPTY,  EMPTY,  BOTH,   BOTH,   BOTH,   BOTH,   BOTH,   BOTH,
//         EMPTY, SOURCE, EMPTY,  EMPTY,  EMPTY,  BOTH,   BOTH,   BOTH,   BOTH,
//         BOTH, BOTH,   EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  BOTH,   BOTH,
//         BOTH, BOTH,   BOTH,   BOTH,   EMPTY,  EMPTY,  EMPTY,
//     },
//     .width = 21,
//     .height = 11,
// };

int main() {
  seed_set_to_time();

  struct Grid *grid = grid_factory.generate_compact_target_region_by_imbalance(
      20, 20, 10, -200);
  // struct Grid *grid = grid_1;
  // struct Grid *grid = grid_2;
  printf("Initial grid:\n");
  grid_print(grid);

  const RedRecParams params = {
      .linear_solver =
          &(struct LinearSolver){
              .solve = linear_solve_aggarwal,
              .params = NULL,
              .name = "",
          },
  };

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
