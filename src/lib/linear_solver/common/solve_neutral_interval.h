#pragma once

#include <stdbool.h>

#include "../../interval/interval.h"
#include "../../interval/mapping.h"
#include "../../utils/range.h"

struct Mapping *solve_neutral_interval(const struct Interval *interval,
                                       const bool *exclusion_array,
                                       int target_num);

void solve_neutral_interval_range(const struct Interval *interval,
                                  const bool *exclusion_array,
                                  struct Range range, int first_source,
                                  int first_target, struct Mapping *mapping);

void solve_neutral_interval_half(const struct Interval *interval,
                                 const bool *exclusion_array,
                                 struct Mapping *mapping, bool solve_source,
                                 bool solve_first_half);
