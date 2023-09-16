#pragma once

#include "../interval/interval.h"

struct Pair {
  unsigned int source;
  unsigned int target;
};

struct Mapping {
  struct Pair *pairs;
  int pair_count;
};

struct Mapping *solve_interval(struct Interval *interval);
void mapping_free(struct Mapping *mapping);

char *mapping_to_string(struct Mapping *mapping);
void mapping_print(struct Mapping *mapping);
