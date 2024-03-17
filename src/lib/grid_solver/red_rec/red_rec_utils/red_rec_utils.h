#pragma once

#include "../../../linear_solver/linear_solver.h"
#include "../../grid_solver.h"

struct ColumnPair {
  int donor_index;
  int receiver_index;
  int exchanged_sources_num;
  int receiver_deficit;
};

bool column_pair_exists(struct ColumnPair pair);

/**
 * Returns the best column pair, ensuring that the pair only contains
 * solved columns between the donor and the receiver
 *
 * If no redistribution can be done anymore, returns a nonexistent pair
 */
struct ColumnPair column_pair_get_best(const struct Grid *grid,
                                       const struct Counts *column_counts);

struct ReceiverDelayedMoves {
  struct ColumnPair *pairs;
  int length;
};

struct DelayedMoves {
  struct ReceiverDelayedMoves *array;
  int grid_width;
};

struct DelayedMoves delayed_moves_new(const struct Grid *grid);
void delayed_moves_free(struct DelayedMoves delayed_moves);
void delayed_moves_add(struct DelayedMoves delayed_moves,
                       struct ColumnPair column_pair);

/**
 * Returns a row index indicating the location in the receiver at which the
 * sources moving up and down must meet.
 */
int get_receiver_pivot(const struct Grid *grid,
                       const struct ReceiverDelayedMoves delayed_moves,
                       struct Range target_range,
                       const struct LinearSolver *linear_solver);

void execute_move(struct Grid *grid, struct Reconfiguration *reconfiguration,
                  struct Range *fixed_sources_range,
                  struct ColumnPair column_pair);

void solve_donor(struct Grid *grid, struct Reconfiguration *reconfiguration,
                 int column_index, const struct LinearSolver *linear_solver);

struct ReceiverOrder {
  int *receiver_indexes;
  int receiver_num;
};

struct ReceiverOrder *receiver_order_new(int max_receiver_num);
void receiver_order_free(struct ReceiverOrder *receiver_order);
void receiver_order_push(struct ReceiverOrder *receiver_order,
                         int receiver_index);
