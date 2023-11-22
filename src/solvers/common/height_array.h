#pragma once

#include "../../interval/interval.h"
#include "../../mapping/mapping.h"

int *get_height_array(const struct Interval *interval);

int get_imbalance_from_height_array(const struct Interval *interval,
                                    const int *height_array);
