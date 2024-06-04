#include <stdlib.h>

#include "./column_pair_array.h"

/**
 * Returns a positive number if the first element is greater than the second,
 * and vice versa
 */
static int compare_column_pairs(struct ColumnPair a, struct ColumnPair b) {
  /**
   * Comparisons by priority
   * 1. maximizes the number of exchanged sources number
   * 2. minimizes the number of columns between the donor and the receiver
   * 3. maximizes the receiver surplus (closest to saturation)
   */

  int a_exchange_num = get_exchange_num(a);
  int b_exchange_num = get_exchange_num(b);
  if (a_exchange_num != b_exchange_num) {
    return a_exchange_num - b_exchange_num;
  }

  int column_distance_a = abs(a.donor_index - a.receiver_index);
  int column_distance_b = abs(b.donor_index - b.receiver_index);
  if (column_distance_a != column_distance_b) {
    return column_distance_b - column_distance_a;
  }

  return b.receiver_deficit - a.receiver_deficit;
}

static bool column_pair_exists(struct ColumnPair pair) {
  return get_exchange_num(pair) > 0;
}

struct ColumnPairPQ column_pair_array_new(struct Counts *column_counts,
                                          int grid_width) {
  return (struct ColumnPairPQ){
      .heap = NULL,
      .column_counts = column_counts,
      .pair_num = 0,
      .grid_width = grid_width,
      .pq_type = ARRAY_PRIORITY_QUEUE,
  };
}

static struct ColumnPair
column_pair_array_get_best(const struct ColumnPairPQ *pq) {
  struct ColumnPair best_column_pair = {
      .donor_index = -1,
      .receiver_index = -1,
      .donor_surplus = 0,
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
      struct ColumnPair pair = previous_column_is_receiver
                                   ? ((struct ColumnPair){
                                         .donor_index = current_column,
                                         .receiver_index = previous_column,
                                         .donor_surplus = current_imbalance,
                                         .receiver_deficit = previous_imbalance,
                                     })
                                   : ((struct ColumnPair){
                                         .donor_index = previous_column,
                                         .receiver_index = current_column,
                                         .donor_surplus = previous_imbalance,
                                         .receiver_deficit = current_imbalance,
                                     });

      if (!column_pair_exists(best_column_pair) ||
          compare_column_pairs(pair, best_column_pair) > 0) {
        best_column_pair = pair;
      }
    }

    previous_column = current_column;
    previous_imbalance =
        counts_get_imbalance(pq->column_counts[previous_column]);
  }

  return best_column_pair;
}

bool column_pair_array_is_empty(const struct ColumnPairPQ *pq) {
  return get_exchange_num(column_pair_array_get_best(pq)) <= 0;
}

struct ColumnPair column_pair_array_pop(const struct ColumnPairPQ *pq) {
  struct ColumnPair best_column_pair = column_pair_array_get_best(pq);

  if (column_pair_exists(best_column_pair)) {
    int exchange_num = get_exchange_num(best_column_pair);
    pq->column_counts[best_column_pair.donor_index].source_num -= exchange_num;
    pq->column_counts[best_column_pair.receiver_index].source_num +=
        exchange_num;
  }

  return best_column_pair;
}

void column_pair_array_free(struct ColumnPairPQ *pq) {
  (void)pq; // to avoid unused parameter warning
}
