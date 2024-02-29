#pragma once

#include "../../../grid/grid.h"
#include "../../../linear_solver/linear_solver.h"
#include "../../common/delayed_moves.h"
#include "../../grid_solver.h"

/**
 * Returns a row index indicating the location in the receiver at which the
 * sources moving up and down must meet.
 */
int get_receiver_pivot(const struct Grid *grid,
                       const struct DelayedMoves *delayed_moves,
                       struct Range target_range, struct ColumnPair column_pair,
                       const struct LinearSolver *linear_solver);

void execute_move(struct Grid *grid, struct Reconfiguration *reconfiguration,
                  struct Range *fixed_sources_range,
                  struct ColumnPair column_pair);

void solve_self_sufficient_column(struct Grid *grid,
                                  struct Reconfiguration *reconfiguration,
                                  int column_index,
                                  const struct LinearSolver *linear_solver);
