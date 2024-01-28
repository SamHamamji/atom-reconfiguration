#include <stdbool.h>
#include <stdlib.h>

#include "./grid.h"

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
bool grid_target_region_is_compact(const struct Grid *grid) {
  if (grid->height == 0 || grid->width == 0) {
    return true;
  }

  // Check that each row contains either all targets or all non targets
  for (int row = 0; row < grid->height; row++) {
    for (int col = 1; col < grid->width; col++) {
      if (grid_get_point(grid, (struct Coordinates){col, row}).is_target !=
          grid_get_point(grid, (struct Coordinates){col - 1, row}).is_target) {
        return false;
      }
    }
  }

  return interval_target_region_is_compact(&(struct Interval){
      .array = grid_get_column(grid, 0),
      .length = grid->height,
  });
}

struct Grid *grid_get_copy(const struct Grid *grid) {
  struct Grid *copy = malloc(sizeof(struct Grid));
  copy->width = grid->width;
  copy->height = grid->height;
  copy->elements = malloc(grid->width * grid->height * sizeof(struct Point));
  for (int i = 0; i < grid->width * grid->height; i++) {
    copy->elements[i] = grid->elements[i];
  }
  return copy;
}

void grid_free(struct Grid *grid) {
  free(grid->elements);
  free(grid);
}

struct Point *grid_get_column(const struct Grid *grid, int col) {
  return &grid->elements[col * grid->height];
}

struct Point grid_get_point(const struct Grid *grid,
                            struct Coordinates coords) {
  return grid->elements[coords.col * grid->height + coords.row];
}

static void grid_set_point(struct Grid *grid, struct Coordinates coords,
                           struct Point value) {
  grid->elements[coords.col * grid->height + coords.row] = value;
}

struct Counts *grid_get_column_counts(const struct Grid *grid) {
  struct Counts *column_counts = calloc(grid->width, sizeof(struct Counts));

  for (int i = 0; i < grid->width; i++) {
    column_counts[i] = interval_get_counts(&(struct Interval){
        .array = grid_get_column(grid, i),
        .length = grid->height,
    });
  }

  return column_counts;
}

bool grid_is_solved(const struct Grid *grid) {
  for (int i = 0; i < grid->width * grid->height; i++) {
    if (grid->elements[i].is_target && !grid->elements[i].is_source) {
      return false;
    }
  }
  return true;
}

static struct Grid *generate_grid(int width, int height) {
  struct Grid *grid = malloc(sizeof(struct Grid));
  grid->width = width;
  grid->height = height;
  grid->elements = malloc(width * height * sizeof(struct Point));
  for (int i = 0; i < width * height; i++) {
    grid->elements[i].is_source = (bool)(rand() % 2);
    grid->elements[i].is_target = (bool)(rand() % 2);
  }
  return grid;
}

static struct Grid *generate_compact_target_region_grid(int width, int height) {
  struct Grid *grid = malloc(sizeof(struct Grid));
  grid->width = width;
  grid->height = height;
  grid->elements = malloc(width * height * sizeof(struct Point));
  for (int col = 0; col < width; col++) {
    for (int row = 0; row < height; row++) {
      grid_set_point(
          grid, (struct Coordinates){col, row},
          (struct Point){
              .is_source = (bool)(rand() % 2),
              .is_target = (height / 4 < row) && (row < height * 3 / 4),
          });
    }
  }
  return grid;
}

const struct GridFactory grid_factory = {
    .generate = generate_grid,
    .generate_compact_target_region = generate_compact_target_region_grid,
};
