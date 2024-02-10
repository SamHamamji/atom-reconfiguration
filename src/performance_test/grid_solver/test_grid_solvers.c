#include "./test_grid_solvers.h"
#include "../../lib/grid_solver/grid_solver.h"
#include "../../lib/utils/timer.h"
#include "performance.h"

static double
test_grid_solver_performance_on_grid(const struct GridSolver *grid_solver,
                                     const struct Grid *grid) {
  struct Timer timer;

  timer_start(&timer);
  struct Reconfiguration *reconfiguration =
      grid_solver->solve(grid, grid_solver->params);
  timer_stop(&timer);

  reconfiguration_free(reconfiguration);

  return timer_get_seconds(&timer);
}

struct PerformanceArray *
test_grid_solvers_performance(const struct PerformanceTestCasesConfig *config) {
  struct PerformanceArray *performance_array = performance_array_new(
      config->grid_solvers_num * config->grid_sizes_num *
      config->imbalance_percentages_num * config->repetition_num);

  int test_case_index = 0;
  for (int i = 0; i < config->grid_sizes_num; i++) {
    for (int j = 0; j < config->imbalance_percentages_num; j++) {
      for (int k = 0; k < config->repetition_num; k++) {
        struct Grid *grid = config->grid_generator(
            config->grid_sizes[i], config->imbalance_percentages[j]);
        for (int grid_solver_index = 0;
             grid_solver_index < config->grid_solvers_num;
             grid_solver_index++) {
          double time_taken = test_grid_solver_performance_on_grid(
              config->grid_solvers[grid_solver_index], grid);

          performance_array->performances[test_case_index] =
              (struct Performance){
                  .grid_solver = config->grid_solvers[grid_solver_index],
                  .grid_size = {.height = grid->height, .width = grid->width},
                  .imbalance_percentage = config->imbalance_percentages[j],
                  .time_taken = time_taken,
              };
          test_case_index++;
        }
        grid_free(grid);
      }
    }
  }
  return performance_array;
}
