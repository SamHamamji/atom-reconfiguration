#include <stdlib.h>

#include "height_array.h"

int *get_height_array(const struct Interval *interval) {
  if (interval->length < 0) {
    return NULL;
  }

  int *height_array = malloc(interval->length * sizeof(int));
  height_array[0] = (int)interval->array[0].is_source;

  for (int i = 1; i < interval->length; i++) {
    height_array[i] = height_array[i - 1] + (int)interval->array[i].is_source -
                      (int)interval->array[i - 1].is_target;
  }

  return height_array;
}

inline int get_imbalance_from_height_array(const struct Interval *interval,
                                           const int *height_array) {
  return height_array[interval->length - 1] -
         (int)interval->array[interval->length - 1].is_target;
}
