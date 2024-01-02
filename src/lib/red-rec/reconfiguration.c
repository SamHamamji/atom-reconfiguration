#include <stdbool.h>
#include <stdlib.h>

#include "../grid/grid.h"
#include "reconfiguration.h"

static bool coordinates_are_equal(struct Coordinates a, struct Coordinates b) {
  return a.col == b.col && a.row == b.row;
}

void reconfiguration_free(struct Reconfiguration *reconfiguration) {
  free(reconfiguration->moves);
  free(reconfiguration);
}

void reconfiguration_apply(const struct Reconfiguration *reconfiguration,
                           struct Grid *grid) {
  bool *move_is_complete = calloc(reconfiguration->move_count, sizeof(bool));
  int move_counter = 0;

  while (move_counter < reconfiguration->move_count) {
    for (int i = 0; i < reconfiguration->move_count; i++) {
      if (move_is_complete[i]) {
        continue;
      }

      struct Coordinates destination = reconfiguration->moves[i].destination;
      struct Coordinates origin = reconfiguration->moves[i].origin;

      // printf("Moving from (%d, %d) to (%d, %d)\n", origin.col, origin.row,
      //        destination.col, destination.row);

      if (!coordinates_are_equal(destination, origin)) {
        if (grid_get_point(grid, destination.col, destination.row).is_source ||
            !grid_get_point(grid, origin.col, origin.row).is_source) {
          continue;
        }
        grid_set_source(grid, origin.col, origin.row, false);
        grid_set_source(grid, destination.col, destination.row, true);
      }

      move_is_complete[i] = true;
      move_counter++;
      // printf("Move %d/%d\n", move_counter, reconfiguration->move_count);
    }
  }

  free(move_is_complete);
}

void reconfiguration_add_move(struct Reconfiguration *reconfiguration,
                              struct Move move) {
  reconfiguration->moves[reconfiguration->move_count] = move;
  reconfiguration->move_count++;
}
