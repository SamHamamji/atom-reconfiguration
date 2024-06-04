#pragma once

#include "../red_rec_utils.h"

struct ColumnPairPQ column_pair_heap_new(struct Counts *column_counts,
                                         int grid_width);

bool column_pair_heap_is_empty(const struct ColumnPairPQ *pq);

struct ColumnPair column_pair_heap_pop(struct ColumnPairPQ *pq);

void column_pair_heap_print(const struct ColumnPairPQ *pq);

void column_pair_heap_free(struct ColumnPairPQ *pq);
