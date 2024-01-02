#pragma once

#define NO_RIGHT_PARTNER -1
#define NO_CHAIN_START -1

#include "../../interval/interval.h"

struct AlternatingChains {
  int *right_partners;
  int *chain_start_indexes;
};

void alternating_chains_free(struct AlternatingChains *chains);

void alternating_chains_compute_range(const struct Interval *interval,
                                      struct AlternatingChains *chains,
                                      struct Range range);
struct AlternatingChains *
alternating_chains_get(const struct Interval *interval, int imbalance);

int alternating_chains_get_exclusion(const struct AlternatingChains *chains,
                                     int chain_index, int max_exclusion_index);

int *alternating_chains_get_exclusion_from_range(
    const struct AlternatingChains *chains, struct Range range,
    int interval_length);

bool *
alternating_chains_get_exclusion_array(const struct AlternatingChains *chains,
                                       int interval_length, int imbalance);
