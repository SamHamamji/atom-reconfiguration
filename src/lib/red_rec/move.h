#pragma once

#include "../grid/grid.h"

struct Move {
  struct Coordinates origin;
  struct Coordinates destination;
};

// Applies move to grid, assumes move is valid.
void move_apply(struct Grid *grid, struct Move move);

// Checks if move is unobstructed.
bool move_is_valid(const struct Grid *grid, struct Move move);
