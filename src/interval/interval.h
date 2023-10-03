#pragma once

#define NUM_POINT_TYPES 3
typedef enum {
  EMPTY,
  TARGET,
  SOURCE,
} Point;

struct Interval {
  Point *array;
  int size;
};

void interval_free(struct Interval *interval);

void interval_print(const struct Interval *interval);

struct IntervalFactory {
  struct Interval *(*generate_interval)(const int size, const int target_num,
                                        const int source_num);
  struct Interval *(*generate_randomized_interval)(const int size);
  struct Interval *(*new_interval)(const Point *const points, const int size);
};

extern const struct IntervalFactory interval_factory;
