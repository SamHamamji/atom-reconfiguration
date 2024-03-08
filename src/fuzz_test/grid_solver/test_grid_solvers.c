#include <stdio.h>
#include <stdlib.h>

#include "../../lib/grid/reconfiguration.h"
#include "../../lib/grid_solver/grid_solver.h"
#include "../../lib/utils/colors.h"
#include "../../lib/utils/timer.h"
#include "./test_grid_solvers.h"

static void print_failed_test_case(const struct Grid *initial_grid,
                                   const struct Grid *grid,
                                   const struct GridSolver *solver,
                                   int test_case_num) {
  printf("Grid is not solved! Failed test case %d.\n", test_case_num);
  printf("Initial grid:\n");
  grid_print(initial_grid);
  printf("Grid from %s:\n", solver->name);
  grid_print(grid);
}

bool fuzz_test_grid_solvers(const struct GridSolversFuzzTestConfig config) {
  printf(BOLD "Testing grid solvers...\n" RESET);

  struct Timer timer;
  timer_start(&timer);

  bool success = true;
  int test_case_num = 0;
  while (success && timer_get_seconds(&timer) < config.time_limit_in_seconds) {
    struct Grid *initial_grid =
        config.grid_generator(get_random_int_in_range(config.width_range),
                              get_random_int_in_range(config.height_range));

    for (int solver_index = 0; solver_index < config.grid_solvers_num;
         solver_index++) {
      const struct GridSolver *solver = config.grid_solvers[solver_index];
      struct Reconfiguration *reconfiguration =
          solver->solve(initial_grid, solver->params);

      struct Grid *grid = grid_get_copy(initial_grid);
      grid_apply_reconfiguration(grid, reconfiguration);

      if (reconfiguration != NULL && !grid_is_solved(grid)) {
        success = false;
        print_failed_test_case(initial_grid, grid, solver, test_case_num);
      }

      reconfiguration_free(reconfiguration);
      grid_free(grid);
    }

    grid_free(initial_grid);

    test_case_num++;
    timer_stop(&timer);
  }

  if (success) {
    printf("%d red-rec tests passed in %.2f seconds!\n", test_case_num,
           config.time_limit_in_seconds);
  } else {
    printf("🔴 Failed red-rec test case %d.\n", test_case_num);
  }

  return success;
}
