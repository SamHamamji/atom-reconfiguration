#include <stdio.h>

#include "./reconfiguration.h"

void reconfiguration_print(const struct Reconfiguration *reconfiguration) {
  if (reconfiguration == NULL) {
    printf("NULL reconfiguration\n");
    return;
  }
  for (int i = 0; i < reconfiguration_get_move_count(reconfiguration); i++) {
    struct Move move = reconfiguration_get_move(reconfiguration, i);
    printf("Move %d: (%d, %d) -> (%d, %d)\n", i, move.origin.col,
           move.origin.row, move.destination.col, move.destination.row);
  }
}
