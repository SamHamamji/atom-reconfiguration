#pragma once

#include "../interval/interval.h"
#include "../mapping/mapping.h"

struct Mapping *solve_neutral_interval(struct Interval *interval,
                                       int *exclusion_array);

int *get_height_array(struct Interval *interval);
