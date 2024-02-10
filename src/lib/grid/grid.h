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

struct Grid *grid_get_copy(const struct Grid *grid);
void grid_free(struct Grid *grid);

struct Point grid_get_point(const struct Grid *grid, struct Coordinates coords);
struct Point *grid_get_column(const struct Grid *grid, int col);
struct Counts *grid_get_column_counts(const struct Grid *grid);

/**
 * Returns whether the target region is compact, i.e. row contains either all
 * targets or all non targets and rows with target points are consecutive
 *
 * i.e. this is compact:
 * . S . . S
 * . . . S .
 * T T B T T
 * B B T B T
 * T T T T T
 * S S . . S
 * . S . . S
 */
bool grid_target_region_is_compact(const struct Grid *grid);
bool grid_is_solved(const struct Grid *grid);

void grid_print(const struct Grid *grid);

struct GridFactory {
  struct Grid *(*generate)(int width, int height);
  struct Grid *(*generate_compact_target_region)(int width, int height,
                                                 int target_region_height);
  struct Grid *(*generate_compact_target_region_by_imbalance)(
      int width, int height, int target_region_height, int imbalance);
};

extern const struct GridFactory grid_factory;
