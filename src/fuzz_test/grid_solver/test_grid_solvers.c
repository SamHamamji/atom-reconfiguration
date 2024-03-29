#include <stdio.h>
#include <stdlib.h>

#include "../../lib/grid/reconfiguration.h"
#include "../../lib/grid_solver/grid_solver.h"
#include "../../lib/utils/colors.h"
#include "../../lib/utils/timer.h"
#include "./test_grid_solvers.h"

static void
print_failed_test_case(const struct Grid *initial_grid,
                       const struct Grid *output_grid,
                       const struct Grid *reconfigured_grid,
                       const struct Reconfiguration *reconfiguration,
                       const struct GridSolver *solver, int test_case_num) {
  printf("%s failed test case %d.\n", solver->name, test_case_num);
  if (initial_grid->width * initial_grid->height > 1000) {
    printf("Testcase not shown because it is too big (%dx%d grid)\n",
           initial_grid->width, initial_grid->height);
    return;
  }

  printf("Initial grid:\n");
  grid_print(initial_grid);
  printf("Output grid:\n");
  grid_print(output_grid);
  printf("Resulting grid:\n");
  grid_print(reconfigured_grid);
  printf("Reconfiguration:\n");
  reconfiguration_print(reconfiguration);
}

static bool test_grid_solver_on_grid(const struct Grid *initial_grid,
                                     const struct GridSolver *solver,
                                     int test_case_num) {
  struct Grid *grid = grid_get_copy(initial_grid);
  struct Grid *reconfigured_grid = NULL;
  struct Reconfiguration *reconfiguration = solver->solve(grid, solver->params);

  bool success;
  if (!grid_is_solvable(initial_grid)) {
    success = reconfiguration == NULL;
  } else {
    reconfigured_grid = grid_get_copy(initial_grid);
    grid_apply_reconfiguration(reconfigured_grid, reconfiguration);

    success = (reconfiguration != NULL) && grid_is_solved(reconfigured_grid) &&
              grid_equals(grid, reconfigured_grid);
  }

  if (!success) {
    print_failed_test_case(initial_grid, grid, reconfigured_grid,
                           reconfiguration, solver, test_case_num);
  }

  grid_free(grid);
  grid_free(reconfigured_grid);
  reconfiguration_free(reconfiguration);

  return success;
}

bool fuzz_test_grid_solvers(const struct GridSolversFuzzTestConfig config) {
  printf(BOLD "Testing grid solvers...\n" RESET);

  struct Timer timer;
  timer_start(&timer);

  bool success = true;
  int test_case_num = 0;
  while (success && timer_get_seconds(&timer) < config.time_limit_in_seconds) {
    struct Grid *grid =
        config.grid_generator(get_random_int_in_range(config.width_range),
                              get_random_int_in_range(config.height_range));

    for (int solver_index = 0; solver_index < config.grid_solvers_num;
         solver_index++) {
      const struct GridSolver *solver = config.grid_solvers[solver_index];
      if (!test_grid_solver_on_grid(grid, solver, test_case_num)) {
        success = false;
        break;
      }
    }

    grid_free(grid);

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
