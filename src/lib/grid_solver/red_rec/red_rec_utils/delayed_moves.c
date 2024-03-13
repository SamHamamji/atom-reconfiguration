#include <stdlib.h>

#include "red_rec_utils.h"

struct DelayedMoves delayed_moves_new(const struct Grid *grid) {
  struct DelayedMoves delayed_moves = {
      .array = calloc(grid->width, sizeof(struct ReceiverDelayedMoves)),
      .grid_width = grid->width,
  };

  return delayed_moves;
}

void delayed_moves_free(struct DelayedMoves delayed_moves) {
  for (int i = 0; i < delayed_moves.grid_width; i++) {
    if (delayed_moves.array[i].pairs != NULL) {
      free(delayed_moves.array[i].pairs);
    }
  }
  free(delayed_moves.array);
}

void delayed_moves_add(struct DelayedMoves delayed_moves,
                       struct ColumnPair column_pair) {

  struct ReceiverDelayedMoves *receiver_moves =
      &delayed_moves.array[column_pair.receiver_index];

  if (receiver_moves->pairs == NULL) {
    receiver_moves->pairs =
        malloc((delayed_moves.grid_width - 1) * sizeof(struct ColumnPair));
  }

  receiver_moves->pairs[receiver_moves->length] = column_pair;
  receiver_moves->length++;
}
