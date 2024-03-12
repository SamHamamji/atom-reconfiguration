#include <stdio.h>
#include <stdlib.h>

#include "../../lib/grid/reconfiguration.h"
#include "../../lib/grid_solver/grid_solver.h"
#include "../../lib/utils/colors.h"
#include "../../lib/utils/timer.h"
#include "./test_grid_solvers.h"

static void print_failed_test_case(const struct Grid *grid,
                                   const struct GridSolver *solver,
                                   int test_case_num) {
  printf("%s failed test case %d.\n", solver->name, test_case_num);
  printf("Initial grid:\n");
  grid_print(grid);
}

static bool test_grid_solver_on_grid(const struct Grid *grid,
                                     const struct GridSolver *solver) {
  struct Grid *grid_input = grid_get_copy(grid);
  struct Reconfiguration *reconfiguration =
      solver->solve(grid_input, solver->params);

  bool success;
  if (!grid_is_solvable(grid)) {
    success = reconfiguration == NULL;
  } else {
    struct Grid *grid_test = grid_get_copy(grid);
    grid_apply_reconfiguration(grid_test, reconfiguration);

    success = (reconfiguration != NULL) && grid_is_solved(grid_test) &&
              grid_is_solved(grid_input);

    grid_free(grid_test);
  }

  grid_free(grid_input);
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
      if (!test_grid_solver_on_grid(grid, solver)) {
        success = false;
        print_failed_test_case(grid, solver, test_case_num);
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
