#include <stdio.h>
#include <stdlib.h>

#include "interval.h"

const struct {
  Point key;
  char value;
} point_to_string_map[3] = {
    {EMPTY, '.'},
    {TARGET, 'T'},
    {SOURCE, 'S'},
};

char point_to_char(Point point) {
  for (int i = 0;
       i < sizeof(point_to_string_map) / sizeof(point_to_string_map[0]); i++) {
    if (point_to_string_map[i].key == point) {
      return point_to_string_map[i].value;
    }
  }

  fprintf(stderr, "ERROR: point %d is invalid\n", point);
  exit(EXIT_FAILURE);
}

char *interval_to_string(struct Interval *interval) {
  char *string = (char *)malloc(2 * interval->size * sizeof(char));
  for (int i = 0; i < interval->size; i++) {
    sprintf(&string[2 * i], "%c ", point_to_char(interval->array[i]));
  }
  string[2 * interval->size - 1] = '\0';
  return string;
}

void interval_print(struct Interval *interval) {
  char *str = interval_to_string(interval);
  printf("%s\n", str);
  free(str);
}
