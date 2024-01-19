#include <assert.h>

#include "./move.h"

static bool coordinates_are_equal(struct Coordinates a, struct Coordinates b) {
  return a.col == b.col && a.row == b.row;
}

void move_apply(struct Grid *grid, struct Move move) {
  assert(move_is_valid(grid, move));

  grid_set_source(grid, move.origin, false);
  grid_set_source(grid, move.destination, true);
}

bool move_is_valid(const struct Grid *grid, struct Move move) {
  if (!grid_get_point(grid, move.origin).is_source) {
    return false;
  }
  bool horizontal_move = move.origin.row == move.destination.row;
  bool vertical_move = move.origin.col == move.destination.col;
  if (!horizontal_move && !vertical_move) {
    return false;
  }

  struct Coordinates current = move.origin;
  while (!coordinates_are_equal(current, move.destination)) {
    if (horizontal_move) {
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
