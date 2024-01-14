#include <stdbool.h>
#include <stdlib.h>

#include "../interval/mapping.h"
#include "move.h"
#include "reconfiguration.h"

struct Reconfiguration {
  struct Move *moves;
  int move_count;
};

struct Reconfiguration *reconfiguration_new(int max_move_count) {
  struct Reconfiguration *reconfiguration =
      malloc(sizeof(struct Reconfiguration));
  *reconfiguration = (struct Reconfiguration){
      .moves = malloc(max_move_count * sizeof(struct Move)),
      .move_count = 0,
  };
  return reconfiguration;
}

void reconfiguration_free(struct Reconfiguration *reconfiguration) {
  if (reconfiguration == NULL) {
    return;
  }
  free(reconfiguration->moves);
  free(reconfiguration);
}

void reconfiguration_apply_move(struct Reconfiguration *reconfiguration,
                                struct Grid *grid, int index) {
  move_apply(grid, reconfiguration->moves[index]);
}

void reconfiguration_apply_move_range(
    const struct Reconfiguration *reconfiguration, struct Grid *grid,
    struct Range range) {
  for (int i = range.start; i < range.exclusive_end; i++) {
    move_apply(grid, reconfiguration->moves[i]);
  }
}

void reconfiguration_apply_last_moves(
    const struct Reconfiguration *reconfiguration, struct Grid *grid,
    int move_count) {
  reconfiguration_apply_move_range(
      reconfiguration, grid,
      (struct Range){
          .start = reconfiguration->move_count - move_count,
          .exclusive_end = reconfiguration->move_count,
      });
}

void reconfiguration_apply(const struct Reconfiguration *reconfiguration,
                           struct Grid *grid) {
  reconfiguration_apply_move_range(
      reconfiguration, grid,
      (struct Range){
          .start = 0,
          .exclusive_end = reconfiguration->move_count,
      });
}

void reconfiguration_add_move(struct Reconfiguration *reconfiguration,
                              struct Move move) {
  reconfiguration->moves[reconfiguration->move_count] = move;
  reconfiguration->move_count++;
}

struct Move
reconfiguration_get_move(const struct Reconfiguration *reconfiguration,
                         int index) {
  return reconfiguration->moves[index];
}

int reconfiguration_get_move_count(
    const struct Reconfiguration *reconfiguration) {
  return reconfiguration->move_count;
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
        reconfiguration_apply_last_moves(reconfiguration, grid, 1);
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
