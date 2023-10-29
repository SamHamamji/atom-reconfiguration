#pragma once

#include "../interval/interval.h"
#include "../mapping/mapping.h"

struct Mapping *solve_neutral_interval(const struct Interval *interval,
                                       const bool *exclusion_array);

int *get_height_array(const struct Interval *interval);

int get_imbalance(const struct Interval *interval, const int *height_array);
