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

bool grid_target_region_is_compact(const struct Grid *grid);
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
struct Counts *grid_get_column_counts(const struct Grid *grid);
bool grid_is_solved(const struct Grid *grid);

void grid_print(const struct Grid *grid);

struct GridFactory {
  struct Grid *(*generate)(int width, int height);
  struct Grid *(*generate_compact_target_region)(int width, int height);
};

extern const struct GridFactory grid_factory;
