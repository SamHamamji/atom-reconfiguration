#include <stdio.h>
#include <stdlib.h>

#include "interval.h"

struct Interval *new_interval(const Point points[], int size) {
  struct Interval *interval =
      (struct Interval *)malloc(sizeof(struct Interval));

  Point *array = (Point *)malloc(size * sizeof(Point));

  for (int i = 0; i < size; i++) {
    array[i] = points[i];
  }

  *interval = (struct Interval){
      .array = array,
      .size = size,
  };

  return interval;
}

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
  printf("ERROR: %d is invalid\n", point);
  return ' ';
}

char *interval_to_string(struct Interval *interval) {
  char *str = (char *)malloc(2 * interval->size * sizeof(char));
  for (int i = 0; i < interval->size; i++) {
    sprintf(&str[2 * i], "%c ", point_to_char(interval->array[i]));
  }
  str[2 * interval->size - 1] = '\0';
  return str;
}

void interval_free(struct Interval *self) {
  free(self->array);
  free(self);
}