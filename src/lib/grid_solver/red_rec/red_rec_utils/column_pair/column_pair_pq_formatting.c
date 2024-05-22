#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../../../utils/colors.h"
#include "../../../../utils/max_min.h"
#include "../red_rec_utils.h"

static int integer_length(int num) {
  if (num == 0) {
    return 1;
  }

  return (int)log10((double)abs(num)) + 1 + (int)(num < 0);
}

static void column_pair_node_print(const struct ColumnPairPQ *pq, int index) {
  struct ColumnPairNode node = pq->heap[index];

  int left_imbalance =
      counts_get_imbalance(pq->column_counts[node.left_column]);
  int right_imbalance =
      counts_get_imbalance(pq->column_counts[node.right_column]);

  int exchanged_sources = ((left_imbalance >= 0) != (right_imbalance >= 0))
                              ? min(abs(left_imbalance), abs(right_imbalance))
                              : 0;

  printf("%d<-(" BLUE "%d" RESET ": %d-%d, %s%d, %d, %d" RESET ")->%d",
         node.left_pair_index, index, node.left_column, node.right_column,
         (exchanged_sources == 0) ? RED : GREEN, exchanged_sources,
         abs(node.left_column - node.right_column),
         -min(left_imbalance, right_imbalance), node.right_pair_index);
}

void column_pair_pq_print(const struct ColumnPairPQ *pq) {
  printf("               ");
  for (int i = 0; i < pq->grid_width; i++) {
    printf("%*d, ", integer_length(counts_get_imbalance(pq->column_counts[i])),
           i);
  }
  printf("\n");
  printf("column counts: ");
  for (int i = 0; i < pq->grid_width; i++) {
    int x = counts_get_imbalance(pq->column_counts[i]);
    if (x != 0) {
      printf(x > 0 ? GREEN : RED);
    }
    printf("%*d, " RESET, integer_length(i), x);
  }
  printf("\n");

  printf("heap (pair num = %d): ", pq->pair_num);
  for (int i = 0; i < pq->pair_num; i++) {
    if (((i + 1) & (i)) == 0) {
      printf("\n");
    } else {
      printf(" | ");
    }
    column_pair_node_print(pq, i);
  }
  printf("\n");
}
