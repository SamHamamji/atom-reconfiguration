#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "grid.h"

static int integer_length(int num) {
  if (num == 0) {
    return 1;
  }
  return (int)log10((double)abs(num)) + 1 + (num <= 0);
}

void grid_print(const struct Grid *grid) {
  if (grid == NULL) {
    printf("NULL grid\n");
    return;
  }

  if (grid->height == 0 || grid->width == 0) {
    printf("Empty grid\n");
    return;
  }
  const int max_height_index_length = integer_length(grid->height - 1);
  const int max_width_index_length = integer_length(grid->width - 1);

  printf("%-*s ", max_height_index_length, "");
  for (int i = 0; i < grid->width; i++) {
    printf("%-*d ", max_width_index_length, i);
  }
  printf("\n");

  for (int row = 0; row < grid->height; row++) {
    printf("%-*d ", max_height_index_length, row);
    for (int col = 0; col < grid->width; col++) {
      struct Point point = grid_get_point(grid, (struct Coordinates){col, row});
      printf("%s", point_to_string(point));
      printf("%-*s", max_width_index_length, "");
    }
    printf("\n");
  }
}
