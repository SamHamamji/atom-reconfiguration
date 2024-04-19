#include <stdbool.h>
#include <stdlib.h>

#include "../../../utils/max_min.h"
#include "red_rec_utils.h"

/**
 * Returns a positive number if the first element is greater than the second,
 * and vice versa
 */
static int compare_column_pairs(struct ColumnPair a, struct ColumnPair b) {
  /**
   * Comparisons by priority
   * 1. maximizes the number of exchanged tokens
   * 2. minimizes the number of columns between the donor and the receiver
   * 3. maximizes the receiver surplus (closest to saturation)
   */

  if (a.exchanged_sources_num != b.exchanged_sources_num) {
    return a.exchanged_sources_num - b.exchanged_sources_num;
  }

  int column_distance_a = abs(a.donor_index - a.receiver_index);
  int column_distance_b = abs(b.donor_index - b.receiver_index);
  if (column_distance_a != column_distance_b) {
    return column_distance_b - column_distance_a;
  }

  return b.receiver_deficit - a.receiver_deficit;
}

bool column_pair_exists(struct ColumnPair pair) {
  return pair.exchanged_sources_num >= 0;
}

struct ColumnPairPQ column_pair_pq_new(struct Counts *column_counts,
                                       int grid_width) {
  return (struct ColumnPairPQ){
      // .pairs = malloc(grid_width * sizeof(struct ColumnPair)),
      .pairs = NULL,
      .column_counts = column_counts,
      .pair_num = 0,
      .grid_width = grid_width,
  };
}

void column_pair_pq_free(struct ColumnPairPQ *pq) {
  // free(pq->pairs);
}

struct ColumnPair column_pair_pq_pop(struct ColumnPairPQ *pq) {
  struct ColumnPair best_column_pair = {
      .donor_index = -1,
      .receiver_index = -1,
      .exchanged_sources_num = -1,
      .receiver_deficit = 0,
  };

  int previous_column = 0;
  int previous_imbalance =
      (pq->grid_width != 0) ? counts_get_imbalance(pq->column_counts[0]) : 0;
  for (int current_column = 1; current_column < pq->grid_width;
       current_column++) {
    int current_imbalance =
        counts_get_imbalance(pq->column_counts[current_column]);
    if (current_imbalance == 0) {
      continue;
    }

    bool previous_column_is_receiver = previous_imbalance < 0;
    bool current_column_is_receiver = current_imbalance < 0;

    if (previous_column_is_receiver != current_column_is_receiver) {
      struct ColumnPair pair =
          previous_column_is_receiver
              ? (struct ColumnPair){
                    .donor_index = current_column,
                    .receiver_index = previous_column,
                    .exchanged_sources_num =
                        min(-previous_imbalance, current_imbalance),
                    .receiver_deficit = -previous_imbalance,
                }
              : (struct ColumnPair){
                    .donor_index = previous_column,
                    .receiver_index = current_column,
                    .exchanged_sources_num =
                        min(previous_imbalance, -current_imbalance),
                    .receiver_deficit = -current_imbalance,
                };

      if (!column_pair_exists(best_column_pair) ||
          compare_column_pairs(pair, best_column_pair) > 0) {
        best_column_pair = pair;
      }
    }

    previous_column = current_column;
    previous_imbalance =
        counts_get_imbalance(pq->column_counts[previous_column]);
  }

  if (column_pair_exists(best_column_pair)) {
    pq->column_counts[best_column_pair.donor_index].source_num -=
        best_column_pair.exchanged_sources_num;
    pq->column_counts[best_column_pair.receiver_index].source_num +=
        best_column_pair.exchanged_sources_num;
  }

  return best_column_pair;
}
