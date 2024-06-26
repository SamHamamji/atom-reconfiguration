#pragma once

#include "../../../linear_solver/linear_solver.h"
#include "../../grid_solver.h"

struct ColumnPair {
  int donor_index;
  int receiver_index;
  int receiver_deficit;
  int donor_surplus;
};

int get_exchange_num(struct ColumnPair column_pair);

struct ColumnPairNode {
  int left_column;
  int right_column;
  int left_pair_index;
  int right_pair_index;
};

struct ColumnPairPQ {
  struct ColumnPairNode *heap;
  struct Counts *column_counts;
  enum ColumnPairPriorityQueueType pq_type;
  int pair_num;
  int grid_width;
};

struct ColumnPairPQ
column_pair_pq_new(struct Counts *column_counts, int grid_width,
                   enum ColumnPairPriorityQueueType pq_type);

bool column_pair_pq_is_empty(const struct ColumnPairPQ *pq);

/**
 * Returns the best column pair, ensuring that the pair only contains
 * solved columns between the donor and the receiver
 */
struct ColumnPair column_pair_pq_pop(struct ColumnPairPQ *pq);

void column_pair_pq_free(struct ColumnPairPQ *pq);

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
 * Returns the mapping of the last donor sorces to the receiver target region.
 */
struct Mapping *get_last_donor_mapping(
    const struct Grid *grid, const struct ReceiverDelayedMoves delayed_moves,
    struct Range target_range, const struct LinearSolver *linear_solver);

/**
 * Returns the index at which the sources moving right and left meet.
 */
int get_pivot_from_mapping(const struct Mapping *mapping,
                           struct Range target_range);

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
