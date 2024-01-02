#pragma once

struct Coordinates {
  int col;
  int row;
};

struct Move {
  struct Coordinates origin;
  struct Coordinates destination;
};

struct Reconfiguration {
  struct Move *moves;
  int move_count;
};

void reconfiguration_free(struct Reconfiguration *reconfiguration);
void reconfiguration_apply(const struct Reconfiguration *reconfiguration,
                           struct Grid *grid);
void reconfiguration_add_move(struct Reconfiguration *reconfiguration,
                              struct Move move);
