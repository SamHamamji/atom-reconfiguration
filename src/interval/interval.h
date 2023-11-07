#pragma once

#include <stdbool.h>

#define NUM_POINT_TYPES 3

struct Point {
  bool is_target;
  bool is_source;
};

struct Interval {
  struct Point *array;
  int size;
};

void interval_free(struct Interval *interval);

void interval_print(const struct Interval *interval);

struct IntervalFactory {
  struct Interval *(*const generate_interval)(const int size,
                                              const int target_num,
                                              const int source_num);
  struct Interval *(*const generate_randomized_interval)(const int size);
  struct Interval *(*const new_interval)(const struct Point *const points,
                                         const int size);
};

extern const struct IntervalFactory interval_factory;
