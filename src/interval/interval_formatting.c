#include <stdio.h>
#include <stdlib.h>

#include "interval.h"

static const char point_to_char[NUM_POINT_TYPES] = {
    [EMPTY] = '.',
    [SOURCE] = 'S',
    [TARGET] = 'T',
};

static char *interval_to_string(const struct Interval *interval) {
  char *string = (char *)malloc(2 * interval->size * sizeof(char));
  for (int i = 0; i < interval->size; i++) {
    string[2 * i] = point_to_char[interval->array[i]];
    string[2 * i + 1] = ' ';
  }
  string[2 * interval->size - 1] = '\0';
  return string;
}

void interval_print(const struct Interval *interval) {
  char *str = interval_to_string(interval);
  printf("%s\n", str);
  free(str);
}
