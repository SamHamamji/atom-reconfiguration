#include "./red_rec_utils.h"

void solve_donor(struct Grid *grid, struct Reconfiguration *reconfiguration,
                 int column_index, const struct LinearSolver *linear_solver) {
  struct Mapping *mapping = linear_solver->solve(
      &(const struct Interval){
          .array = grid_get_column(grid, column_index),
          .length = grid->height,
      },
      linear_solver->params);

  int initial_move_count = reconfiguration->move_count;

  reconfiguration_add_mapping(reconfiguration, mapping, column_index);

  grid_apply_reconfiguration(
      grid, &(struct Reconfiguration){
                .moves = &reconfiguration->moves[initial_move_count],
                .move_count = reconfiguration->move_count - initial_move_count,
            });

  mapping_free(mapping);
}
