#include "./red_rec_utils.h"

void solve_self_sufficient_column(struct Grid *grid,
                                  struct Reconfiguration *reconfiguration,
                                  int column_index,
                                  const struct LinearSolver *linear_solver) {
  struct Mapping *mapping = linear_solver->solve(
      &(const struct Interval){
          .array = grid_get_column(grid, column_index),
          .length = grid->height,
      },
      linear_solver->params);

  reconfiguration_add_mapping(reconfiguration, grid, mapping, column_index);

  mapping_free(mapping);
}
