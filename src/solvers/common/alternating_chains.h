#pragma once

#define NO_RIGHT_PARTNER -1
#define NO_CHAIN_START -2

#include "../../../src/interval/interval.h"

struct AlternatingChains {
  int *source_right_partners;
  int *target_right_partners;
  int *chain_start_indexes;
  int interval_size;
};

void alternating_chains_free(struct AlternatingChains *chains);

struct AlternatingChains *
get_alternating_chains(const struct Interval *const interval, int imbalance);
