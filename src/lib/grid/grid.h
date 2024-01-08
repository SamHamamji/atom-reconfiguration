#pragma once

#include "../interval/interval.h"

struct Grid {
  struct Point *elements;
  int width;
  int height;
};

struct Coordinates {
  int col;
  int row;
};

struct Grid *grid_new_random(int width, int height);
struct Grid *grid_new_square_compact(int width, int height);
struct Grid *grid_get_copy(const struct Grid *grid);
void grid_free(struct Grid *grid);

struct Point grid_get_point(const struct Grid *grid, struct Coordinates coords);
void grid_set_point(struct Grid *grid, struct Coordinates coords,
                    struct Point value);
void grid_set_source(struct Grid *grid, struct Coordinates coords,
                     bool is_source);
void grid_set_target(struct Grid *grid, struct Coordinates coords,
                     bool is_target);
struct Point *grid_get_column(const struct Grid *grid, int col);
int *grid_get_column_supluses(const struct Grid *grid);

void grid_print(const struct Grid *grid);
