#pragma once

#include <stdbool.h>

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
  int size;
};

struct IntervalCounts {
  int source_num;
  int target_num;
};

void interval_free(struct Interval *interval);

struct IntervalCounts
interval_get_counts_from_slice(const struct Interval *interval, int start,
                               int exclusive_end);
struct IntervalCounts interval_get_counts(const struct Interval *interval);
int get_imbalance_from_counts(struct IntervalCounts counts);

void interval_print(const struct Interval *interval);

struct IntervalFactory {
  struct Interval *(*generate_interval_by_imbalance)(int size, int imbalance);
  struct Interval *(*generate_randomized_interval)(int size);
  struct Interval *(*new_interval)(const struct Point *points, int size);
};

extern const struct IntervalFactory interval_factory;
