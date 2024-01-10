#pragma once

#include "../point/point.h"
#include "../utils/range.h"

/** Represents a line of evenly spaced points */
struct Interval {
  struct Point *array;
  int length;
};

/** Represents a number of sources and targets */
struct Counts {
  int source_num;
  int target_num;
};

void interval_free(struct Interval *interval);

struct Counts interval_get_counts_from_range(const struct Interval *interval,
                                             struct Range range);
struct Counts interval_get_counts(const struct Interval *interval);
int counts_get_imbalance(struct Counts counts);

void interval_print(const struct Interval *interval);

struct IntervalFactory {
  struct Interval *(*generate_interval_by_imbalance)(int length, int imbalance);
  struct Interval *(*generate_randomized_interval)(int length);
  struct Interval *(*new_interval)(const struct Point *points, int length);
};

extern const struct IntervalFactory interval_factory;
