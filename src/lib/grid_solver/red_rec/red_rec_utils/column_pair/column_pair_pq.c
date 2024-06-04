#include "../red_rec_utils.h"
#include "./column_pair_array.h"
#include "./column_pair_heap.h"

struct ColumnPairPQ
column_pair_pq_new(struct Counts *column_counts, int grid_width,
                   enum ColumnPairPriorityQueueType pq_type) {
  return (pq_type == ARRAY_PRIORITY_QUEUE)
             ? column_pair_array_new(column_counts, grid_width)
             : column_pair_heap_new(column_counts, grid_width);
}

bool column_pair_pq_is_empty(const struct ColumnPairPQ *pq) {
  return (pq->pq_type == ARRAY_PRIORITY_QUEUE) ? column_pair_array_is_empty(pq)
                                               : column_pair_heap_is_empty(pq);
}

struct ColumnPair column_pair_pq_pop(struct ColumnPairPQ *pq) {
  return (pq->pq_type == ARRAY_PRIORITY_QUEUE) ? column_pair_array_pop(pq)
                                               : column_pair_heap_pop(pq);
}

void column_pair_pq_free(struct ColumnPairPQ *pq) {
  if (pq->pq_type == ARRAY_PRIORITY_QUEUE) {
    column_pair_array_free(pq);
  } else {
    column_pair_heap_free(pq);
  }
}
