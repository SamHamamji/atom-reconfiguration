#pragma once

#include "../red_rec_utils.h"

struct ColumnPairPQ column_pair_array_new(struct Counts *column_counts,
                                          int grid_width);

bool column_pair_array_is_empty(const struct ColumnPairPQ *pq);

struct ColumnPair column_pair_array_pop(const struct ColumnPairPQ *pq);

void column_pair_array_free(struct ColumnPairPQ *pq);
