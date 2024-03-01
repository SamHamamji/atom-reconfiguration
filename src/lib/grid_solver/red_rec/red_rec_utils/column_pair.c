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

struct ColumnPair column_pair_get_best(const struct Grid *grid,
                                       const struct Counts *column_counts) {
  struct ColumnPair best_column_pair = {
      .donor_index = -1,
      .receiver_index = -1,
      .exchanged_sources_num = -1,
      .receiver_deficit = 0,
  };

  int previous_column = 0;
  for (int current_column = 1; current_column < grid->width; current_column++) {
    if (counts_get_imbalance(column_counts[current_column]) == 0) {
      continue;
    }

    int previous_imbalance =
        counts_get_imbalance(column_counts[previous_column]);
    int current_imbalance = counts_get_imbalance(column_counts[current_column]);

    bool previous_column_is_receiver = previous_imbalance < 0;
    bool current_column_is_receiver = current_imbalance < 0;

    if (previous_column_is_receiver != current_column_is_receiver) {
      struct ColumnPair pair =
          previous_column_is_receiver
              ? ((struct ColumnPair){
                    .donor_index = current_column,
                    .receiver_index = previous_column,
                    .exchanged_sources_num =
                        min(-previous_imbalance, current_imbalance),
                    .receiver_deficit = -previous_imbalance,
                })
              : ((struct ColumnPair){
                    .donor_index = previous_column,
                    .receiver_index = current_column,
                    .exchanged_sources_num =
                        min(previous_imbalance, -current_imbalance),
                    .receiver_deficit = -current_imbalance,
                });

      if (!column_pair_exists(best_column_pair) ||
          compare_column_pairs(pair, best_column_pair) > 0) {
        best_column_pair = pair;
      }
    }
    previous_column = current_column;
  }

  return best_column_pair;
}
