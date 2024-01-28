#include <stdio.h>

#include "./reconfiguration.h"

static bool move_is_in_same_column(struct Move move1, struct Move move2) {
  return (move1.origin.col == move2.origin.col) &&
         (move1.destination.col == move2.destination.col);
}

void reconfiguration_print(const struct Reconfiguration *reconfiguration) {
  if (reconfiguration == NULL) {
    printf("NULL reconfiguration\n");
    return;
  }
  for (int i = 0; i < reconfiguration->move_count; i++) {
    struct Move move = reconfiguration->moves[i];
    printf("(%d, %d) -> (%d, %d)", move.origin.col, move.origin.row,
           move.destination.col, move.destination.row);

    if (i != reconfiguration->move_count - 1 &&
        move_is_in_same_column(move, reconfiguration->moves[i + 1])) {
      printf(", ");
    } else {
      printf("\n");
    }
  }
}
