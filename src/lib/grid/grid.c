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
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      grid_set(grid, x, y,
               (struct Point){
                   .is_source = (bool)(rand() % 2),
                   .is_target = (height / 4 <= y) && (y < height * 3 / 4),
               });
    }
  }
  return grid;
}

void grid_free(struct Grid *grid) {
  free(grid->elements);
  free(grid);
}

struct Point *grid_get_column(const struct Grid *grid, int x) {
  return &grid->elements[x * grid->height];
}

struct Point grid_get_point(const struct Grid *grid, int x, int y) {
  return grid->elements[x * grid->height + y];
}

void grid_set(struct Grid *grid, int x, int y, struct Point value) {
  grid->elements[x * grid->height + y] = value;
}

void grid_set_source(struct Grid *grid, int x, int y, bool is_source) {
  grid->elements[x * grid->height + y].is_source = is_source;
}

void grid_set_target(struct Grid *grid, int x, int y, bool is_target) {
  grid->elements[x * grid->height + y].is_target = is_target;
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
