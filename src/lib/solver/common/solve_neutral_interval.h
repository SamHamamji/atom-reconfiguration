#pragma once

#include <stdbool.h>

#include "../../interval/interval.h"
#include "../../mapping/mapping.h"

struct Mapping *solve_neutral_interval(const struct Interval *interval,
                                       const bool *exclusion_array,
                                       int target_num);

void solve_neutral_interval_slice(const struct Interval *interval,
                                  const bool *exclusion_array,
                                  struct Mapping *mapping, bool solve_source,
                                  bool solve_first_half);
