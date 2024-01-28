#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../interval/mapping.h"
#include "reconfiguration.h"

struct Reconfiguration *reconfiguration_new(int max_move_count) {
  assert(max_move_count >= 0);

  struct Reconfiguration *reconfiguration =
      malloc(sizeof(struct Reconfiguration));
  *reconfiguration = (struct Reconfiguration){
      .moves = malloc(max_move_count * sizeof(struct Move)),
      .move_count = 0,
  };
  return reconfiguration;
}

void reconfiguration_add_move(struct Reconfiguration *reconfiguration,
                              struct Move move) {
  reconfiguration->moves[reconfiguration->move_count] = move;
  reconfiguration->move_count++;
}

void reconfiguration_add_mapping(struct Reconfiguration *reconfiguration,
                                 struct Grid *grid,
                                 const struct Mapping *mapping,
                                 int column_index) {
  bool *move_is_complete = calloc(mapping->pair_count, sizeof(bool));
  bool move_executed = true;
  while (move_executed) {
    move_executed = false;
    for (int i = 0; i < mapping->pair_count; i++) {
      struct Move current_move = (struct Move){
          .origin = {.col = column_index, .row = mapping->pairs[i].source},
          .destination = {.col = column_index, .row = mapping->pairs[i].target},
      };
      if (!move_is_complete[i] && move_is_valid(grid, current_move)) {
        reconfiguration_add_move(reconfiguration, current_move);
        grid_apply_move(grid, current_move);
        move_is_complete[i] = true;
        move_executed = true;
      }
    }
    if (!move_executed) {
      break;
    }
  }

  free(move_is_complete);
}

void reconfiguration_filter_identical(struct Reconfiguration *reconfiguration) {
  int filtered_i = 0;
  for (int i = 0; i < reconfiguration->move_count; i++) {
    if (reconfiguration->moves[i].destination.col ==
            reconfiguration->moves[i].origin.col &&
        reconfiguration->moves[i].destination.row ==
            reconfiguration->moves[i].origin.row) {
      continue;
    }
    reconfiguration->moves[filtered_i] = reconfiguration->moves[i];
    filtered_i++;
  }
  reconfiguration->move_count = filtered_i;
}

void reconfiguration_free(struct Reconfiguration *reconfiguration) {
  if (reconfiguration == NULL) {
    return;
  }
  free(reconfiguration->moves);
  free(reconfiguration);
}

static bool coordinates_are_equal(struct Coordinates a, struct Coordinates b) {
  return a.col == b.col && a.row == b.row;
}

bool move_is_valid(const struct Grid *grid, struct Move move) {
  if (!grid_get_point(grid, move.origin).is_source) {
    return false;
  }
  bool move_is_horizontal = move.origin.row == move.destination.row;
  bool move_is_vertical = move.origin.col == move.destination.col;
  if (!move_is_horizontal && !move_is_vertical) {
    return false;
  }

  struct Coordinates current = move.origin;
  while (!coordinates_are_equal(current, move.destination)) {
    if (move_is_horizontal) {
      current.col += move.destination.col > move.origin.col ? 1 : -1;
    } else {
      current.row += move.destination.row > move.origin.row ? 1 : -1;
    }
    if (grid_get_point(grid, current).is_source) {
      return false;
    }
  }

  return true;
}

void grid_apply_move(struct Grid *grid, struct Move move) {
  assert(move_is_valid(grid, move));

  grid->elements[move.origin.col * grid->height + move.origin.row].is_source =
      false;
  grid->elements[move.destination.col * grid->height + move.destination.row]
      .is_source = true;
}

void grid_apply_move_range(struct Grid *grid,
                           const struct Reconfiguration *reconfiguration,
                           struct Range range) {
  for (int i = range.start; i < range.exclusive_end; i++) {
    grid_apply_move(grid, reconfiguration->moves[i]);
  }
}

void grid_apply_reconfiguration(struct Grid *grid,
                                const struct Reconfiguration *reconfiguration) {
  if (reconfiguration == NULL) {
    return;
  }
  grid_apply_move_range(grid, reconfiguration,
                        (struct Range){
                            .start = 0,
                            .exclusive_end = reconfiguration->move_count,
                        });
}
