#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

#include "./grid.h"

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

struct Range grid_get_compact_target_region_range(const struct Grid *grid) {
  assert(grid_target_region_is_compact(grid));

  struct Range target_range = {
      .start = INT_MAX,
      .exclusive_end = INT_MIN,
  };

  if (grid->width == 0 || grid->height == 0) {
    return target_range;
  }

  struct Point *column = grid_get_column(grid, 0);

  int i = 0;
  while (i < grid->height && !column[i].is_target) {
    i++;
  }
  target_range.start = i;
  while (i < grid->height && column[i].is_target) {
    i++;
  }
  target_range.exclusive_end = i;
  return target_range;
}

bool grid_is_solvable(const struct Grid *grid) {
  int total_imbalance = 0;
  for (int i = 0; i < grid->width; i++) {
    total_imbalance +=
        counts_get_imbalance(interval_get_counts(&(struct Interval){
            .array = grid_get_column(grid, i),
            .length = grid->height,
        }));
  }
  return total_imbalance >= 0;
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

static struct Grid *
generate_compact_target_region_grid(int width, int height,
                                    int target_region_height) {
  assert(target_region_height <= height);
  struct Grid *grid = malloc(sizeof(struct Grid));
  *grid = (struct Grid){
      .elements = malloc(width * height * sizeof(struct Point)),
      .width = width,
      .height = height,
  };

  for (int col = 0; col < width; col++) {
    for (int row = 0; row < height; row++) {
      grid_set_point(
          grid, (struct Coordinates){col, row},
          (struct Point){
              .is_source = (bool)(rand() % 2),
              .is_target = ((height - target_region_height) / 2 <= row) &&
                           (row < (height + target_region_height) / 2),
          });
    }
  }
  return grid;
}

static void grid_shuffle_sources(struct Grid *grid) {
  int grid_size = grid->width * grid->height;
  for (int i = 0; i < grid_size; i++) {
    bool current_source = grid->elements[i].is_source;
    int source_index = rand() % grid_size;
    grid->elements[i].is_source = grid->elements[source_index].is_source;
    grid->elements[source_index].is_source = current_source;
  }
}

static struct Grid *generate_compact_target_region_grid_by_imbalance(
    int width, int height, int target_region_height, int imbalance) {
  int source_num = target_region_height * width + imbalance;

  assert(0 <= source_num && source_num <= width * height);

  struct Grid *grid = malloc(sizeof(struct Grid));
  *grid = (struct Grid){
      .elements = malloc(width * height * sizeof(struct Point)),
      .width = width,
      .height = height,
  };

  for (int col = 0; col < width; col++) {
    for (int row = 0; row < height; row++) {
      grid_set_point(
          grid, (struct Coordinates){col, row},
          (struct Point){
              .is_source = (bool)(row * width + col < source_num),
              .is_target = ((height - target_region_height) / 2 <= row) &&
                           (row < (height + target_region_height) / 2),
          });
    }
  }

  grid_shuffle_sources(grid);

  return grid;
}

const struct GridFactory grid_factory = {
    .generate = generate_grid,
    .generate_compact_target_region = generate_compact_target_region_grid,
    .generate_compact_target_region_by_imbalance =
        generate_compact_target_region_grid_by_imbalance,
};
