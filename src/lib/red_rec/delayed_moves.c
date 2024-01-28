#include <assert.h>
#include <stdlib.h>

#include "./delayed_moves.h"

struct DelayedMoves *delayed_moves_new(int max_length) {
  assert(max_length >= 0);
  struct DelayedMoves *delayed_moves = malloc(sizeof(struct DelayedMoves));
  *delayed_moves = (struct DelayedMoves){
      .array = malloc(max_length * sizeof(struct ColumnPair)),
      .length = 0,
  };

  return delayed_moves;
}

void delayed_moves_free(struct DelayedMoves *delayed_moves) {
  free(delayed_moves->array);
  free(delayed_moves);
}

void delayed_moves_add(struct DelayedMoves *delayed_moves,
                       struct ColumnPair column_pair) {
  delayed_moves->array[delayed_moves->length] = column_pair;
  delayed_moves->length++;
}
