#pragma once

#include "column_pair.h"

struct DelayedMoves {
  struct ColumnPair *array;
  int length;
};

struct DelayedMoves *delayed_moves_new(int max_length);
void delayed_moves_free(struct DelayedMoves *delayed_moves);
void delayed_moves_add(struct DelayedMoves *delayed_moves,
                       struct ColumnPair column_pair);
