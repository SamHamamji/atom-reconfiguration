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
  const int max_height_index_length = integer_length(grid->height - 1);
  const int max_width_index_length = integer_length(grid->width - 1);

  printf("%-*s ", max_height_index_length, "");
  for (int i = 0; i < grid->width; i++) {
    printf("%-*d ", max_width_index_length, i);
  }
  printf("\n");

  for (int y = 0; y < grid->height; y++) {
    printf("%-*d ", max_height_index_length, y);
    for (int x = 0; x < grid->width; x++) {
      printf("%s", point_to_string(grid_get_point(grid, x, y)));
      printf("%-*s", max_height_index_length, "");
    }
    printf("\n");
  }
}
