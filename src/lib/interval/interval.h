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

struct Counts interval_get_counts_from_range(const struct Interval *interval,
                                             struct Range range);
struct Counts interval_get_counts(const struct Interval *interval);
int counts_get_imbalance(struct Counts counts);

bool interval_target_region_is_compact(const struct Interval *interval);

void interval_print(const struct Interval *interval);
void interval_free(struct Interval *interval);

struct IntervalFactory {
  struct Interval *(*generate)(int length);
  struct Interval *(*generate_by_imbalance)(int length, int imbalance);
  struct Interval *(*generate_compact_target_region)(int length, int imbalance);
  struct Interval *(*new_interval)(int length);
};

extern const struct IntervalFactory interval_factory;
