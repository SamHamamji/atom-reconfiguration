#pragma once

#include <stdbool.h>

struct Point {
  bool is_target;
  bool is_source;
};

#define EMPTY                                                                  \
  (struct Point) { .is_target = false, .is_source = false }
#define SOURCE                                                                 \
  (struct Point) { .is_target = false, .is_source = true }
#define TARGET                                                                 \
  (struct Point) { .is_target = true, .is_source = false }
#define BOTH                                                                   \
  (struct Point) { .is_target = true, .is_source = true }

struct Interval {
  struct Point *array;
  int size;
};

void interval_free(struct Interval *interval);

int interval_get_imbalance(const struct Interval *interval);

void interval_print(const struct Interval *interval);

struct IntervalFactory {
  struct Interval *(*const generate_interval_by_imbalance)(const int size,
                                                           const int imbalance);
  struct Interval *(*const generate_randomized_interval)(const int size);
  struct Interval *(*const new_interval)(const struct Point *const points,
                                         const int size);
};

extern const struct IntervalFactory interval_factory;
