#include <stdlib.h>

#include "./grid.h"

struct Grid *grid_new_random(int width, int height) {
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

struct Grid *grid_new_square_compact(int width, int height) {
  struct Grid *grid = malloc(sizeof(struct Grid));
  grid->width = width;
  grid->height = height;
  grid->elements = malloc(width * height * sizeof(struct Point));
  for (int col = 0; col < width; col++) {
    for (int row = 0; row < height; row++) {
      grid_set_source(grid, (struct Coordinates){col, row}, (bool)(rand() % 2));
      grid_set_target(grid, (struct Coordinates){col, row},
                      (height / 4 <= row) && (row < height * 3 / 4));
    }
  }
  return grid;
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

void grid_set_point(struct Grid *grid, struct Coordinates coords,
                    struct Point value) {
  grid->elements[coords.col * grid->height + coords.row] = value;
}

void grid_set_source(struct Grid *grid, struct Coordinates coords,
                     bool is_source) {
  grid->elements[coords.col * grid->height + coords.row].is_source = is_source;
}

void grid_set_target(struct Grid *grid, struct Coordinates coords,
                     bool is_target) {
  grid->elements[coords.col * grid->height + coords.row].is_target = is_target;
}

int *grid_get_column_supluses(const struct Grid *grid) {
  int *column_supluses = calloc(grid->width, sizeof(int));

  for (int i = 0; i < grid->width; i++) {
    struct Point *column = grid_get_column(grid, i);
    for (int j = 0; j < grid->height; j++) {
      column_supluses[i] += (int)column[j].is_source - (int)column[j].is_target;
    }
  }
  return column_supluses;
}
