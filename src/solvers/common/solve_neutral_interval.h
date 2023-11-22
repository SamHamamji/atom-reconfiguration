#pragma once

#include <stdbool.h>

#include "../../interval/interval.h"
#include "../../mapping/mapping.h"

struct Mapping *solve_neutral_interval(const struct Interval *interval,
                                       const bool *exclusion_array);
