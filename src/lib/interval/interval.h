#pragma once

#include <stdbool.h>

#include "../range.h"

struct Point {
  bool is_target;
  bool is_source;
};

#define EMPTY                                                                  \
  { .is_target = false, .is_source = false }
#define SOURCE                                                                 \
  { .is_target = false, .is_source = true }
#define TARGET                                                                 \
  { .is_target = true, .is_source = false }
#define BOTH                                                                   \
  { .is_target = true, .is_source = true }

struct Interval {
  struct Point *array;
  int length;
};

struct IntervalCounts {
  int source_num;
  int target_num;
};

void interval_free(struct Interval *interval);

struct IntervalCounts
interval_get_counts_from_range(const struct Interval *interval,
                               struct Range range);
struct IntervalCounts interval_get_counts(const struct Interval *interval);
int get_imbalance_from_counts(struct IntervalCounts counts);

void interval_print(const struct Interval *interval);

struct IntervalFactory {
  struct Interval *(*generate_interval_by_imbalance)(int length, int imbalance);
  struct Interval *(*generate_randomized_interval)(int length);
  struct Interval *(*new_interval)(const struct Point *points, int length);
};

extern const struct IntervalFactory interval_factory;
