#pragma once

#include "../interval/interval.h"

struct Grid {
  struct Point *elements;
  int width;
  int height;
};

struct Grid *grid_new_random(int width, int height);
struct Grid *grid_new_square_compact(int width, int height);
void grid_free(struct Grid *grid);

struct Point grid_get_point(const struct Grid *grid, int x, int y);
void grid_set(struct Grid *grid, int x, int y, struct Point value);
void grid_set_source(struct Grid *grid, int x, int y, bool is_source);
void grid_set_target(struct Grid *grid, int x, int y, bool is_target);
struct Point *grid_get_column(const struct Grid *grid, int x);
int *grid_get_column_supluses(const struct Grid *grid);

void grid_print(const struct Grid *grid);
