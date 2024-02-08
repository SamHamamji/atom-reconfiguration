#include <stdlib.h>

#include "../../utils/max_min.h"
#include "column_pair.h"

/** Returns a positive number if the first element is greater than the second,
 * and vice versa
 */
static int compare_column_pairs(struct ColumnPair a, struct ColumnPair b,
                                const struct Counts *column_counts) {
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

  int a_receiver_remaining_deficit =
      abs(counts_get_imbalance(column_counts[a.receiver_index]));
  int b_receiver_remaining_deficit =
      abs(counts_get_imbalance(column_counts[b.receiver_index]));

  return b_receiver_remaining_deficit - a_receiver_remaining_deficit;
}

struct ColumnPair column_pair_get_best(struct Grid *grid,
                                       struct Counts *column_counts,
                                       bool *column_is_solved) {
  int previous_column = 0;
  struct ColumnPair best_column_pair = {
      .donor_index = -1,
      .receiver_index = -1,
      .exchanged_sources_num = -1,
  };

  for (int current_column = 1; current_column < grid->width; current_column++) {
    if (column_is_solved[current_column] &&
        counts_get_imbalance(column_counts[current_column]) == 0) {
      continue;
    }
    bool previous_column_is_receiver =
        counts_get_imbalance(column_counts[previous_column]) < 0;
    bool current_column_is_receiver =
        counts_get_imbalance(column_counts[current_column]) < 0;

    if (previous_column_is_receiver != current_column_is_receiver) {
      struct ColumnPair pair = (struct ColumnPair){
          .donor_index =
              previous_column_is_receiver ? current_column : previous_column,
          .receiver_index =
              previous_column_is_receiver ? previous_column : current_column,
          .exchanged_sources_num =
              min(abs(counts_get_imbalance(column_counts[previous_column])),
                  abs(counts_get_imbalance(column_counts[current_column]))),
      };

      if (best_column_pair.donor_index == -1 ||
          best_column_pair.receiver_index == -1 ||
          compare_column_pairs(pair, best_column_pair, column_counts) > 0) {
        best_column_pair = pair;
      }
    }
    previous_column = current_column;
  }
  return best_column_pair;
}
