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

void mapping_free(struct Mapping *mapping);

int pair_equals(const struct Pair a, const struct Pair b);
int mapping_equals(const struct Mapping *a, const struct Mapping *b);

char *mapping_to_string(const struct Mapping *mapping);
void mapping_print(const struct Mapping *mapping);
