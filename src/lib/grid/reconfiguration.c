#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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

void reconfiguration_merge(struct Reconfiguration *reconfiguration,
                           const struct Reconfiguration *other) {
  memcpy(&reconfiguration->moves[reconfiguration->move_count], other->moves,
         other->move_count * sizeof(other->moves[0]));
  reconfiguration->move_count += other->move_count;
}

/**
 * Requirement: Assumes `mapping` is sorted by `source`.
 */
void reconfiguration_add_mapping(struct Reconfiguration *reconfiguration,
                                 struct Grid *grid,
                                 const struct Mapping *mapping,
                                 int column_index) {
  int stack_head = 0;
  struct Pair *delayed_stack =
      malloc(mapping->pair_count * sizeof(struct Pair));

  for (int i = 0; i < mapping->pair_count; i++) {
    bool move_is_valid =
        (mapping->pairs[i].source >= mapping->pairs[i].target) ||
        (i + 1 < mapping->pair_count &&
         mapping->pairs[i + 1].source > mapping->pairs[i].target);
    if (move_is_valid) {
      reconfiguration_add_move(
          reconfiguration,
          (struct Move){
              .origin = {.col = column_index, .row = mapping->pairs[i].source},
              .destination = {.col = column_index,
                              .row = mapping->pairs[i].target},
          });
      grid_apply_move(grid,
                      reconfiguration->moves[reconfiguration->move_count - 1]);
    } else {
      delayed_stack[stack_head] = mapping->pairs[i];
      stack_head++;
    }
  }

  while (stack_head > 0) {
    reconfiguration_add_move(
        reconfiguration,
        (struct Move){
            .origin = {.col = column_index,
                       .row = delayed_stack[stack_head - 1].source},
            .destination = {.col = column_index,
                            .row = delayed_stack[stack_head - 1].target},
        });
    grid_apply_move(grid,
                    reconfiguration->moves[reconfiguration->move_count - 1]);
    stack_head--;
  }
  free(delayed_stack);
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

void grid_apply_reconfiguration(struct Grid *grid,
                                const struct Reconfiguration *reconfiguration) {
  if (reconfiguration == NULL) {
    return;
  }

  for (int i = 0; i < reconfiguration->move_count; i++) {
    grid_apply_move(grid, reconfiguration->moves[i]);
  }
}
