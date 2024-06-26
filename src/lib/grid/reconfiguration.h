#pragma once

#include "../grid/grid.h"
#include "../interval/mapping.h"

struct Move {
  struct Coordinates origin;
  struct Coordinates destination;
};

struct Reconfiguration {
  struct Move *moves;
  int move_count;
};

struct Reconfiguration *reconfiguration_new(int max_move_count);

void reconfiguration_add_move(struct Reconfiguration *reconfiguration,
                              struct Move move);

/**
 * Unobstructs and adds a 1d obstructed `mapping` to `reconfiguration`, mutates
 * `grid` too.
 */
void reconfiguration_add_mapping(struct Reconfiguration *reconfiguration,
                                 const struct Mapping *mapping,
                                 int column_index);

void reconfiguration_filter_identical(struct Reconfiguration *reconfiguration);

void reconfiguration_free(struct Reconfiguration *reconfiguration);

/**
 * Applies move to grid, assumes move is valid.
 */
void grid_apply_move(struct Grid *grid, struct Move move);

void grid_apply_reconfiguration(struct Grid *grid,
                                const struct Reconfiguration *reconfiguration);

void reconfiguration_print(const struct Reconfiguration *reconfiguration);
