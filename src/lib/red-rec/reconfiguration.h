#pragma once

#include "../interval/mapping.h"
#include "./move.h"

struct Reconfiguration; // Opaque type.

struct Reconfiguration *reconfiguration_new(int max_move_count);
void reconfiguration_free(struct Reconfiguration *reconfiguration);
struct Move
reconfiguration_get_move(const struct Reconfiguration *reconfiguration,
                         int index);
int reconfiguration_get_move_count(
    const struct Reconfiguration *reconfiguration);
void reconfiguration_apply_move(struct Reconfiguration *reconfiguration,
                                struct Grid *grid, int index);
void reconfiguration_apply_move_range(
    const struct Reconfiguration *reconfiguration, struct Grid *grid,
    struct Range range);
void reconfiguration_apply_last_moves(
    const struct Reconfiguration *reconfiguration, struct Grid *grid,
    int move_count);
void reconfiguration_apply(const struct Reconfiguration *reconfiguration,
                           struct Grid *grid);
void reconfiguration_add_move(struct Reconfiguration *reconfiguration,
                              struct Move move);
void reconfiguration_print(const struct Reconfiguration *reconfiguration);

/** Unobstructs and adds a 1d obstructed `mapping` to `reconfiguration`, mutates
 * `grid` too. */
void reconfiguration_add_mapping(struct Reconfiguration *reconfiguration,
                                 struct Grid *grid,
                                 const struct Mapping *mapping,
                                 int column_index);
void reconfiguration_filter_identical(struct Reconfiguration *reconfiguration);
