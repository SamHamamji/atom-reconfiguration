#pragma once

#include <stdbool.h>

#include "../../lib/utils/range.h"

struct RedRecFuzzTestConfig {
  struct Grid *(*grid_generator)(int width, int height);
  struct Range height_range;
  struct Range width_range;
  double time_limit_in_seconds;
};

bool fuzz_test_red_rec(const struct RedRecFuzzTestConfig config);
