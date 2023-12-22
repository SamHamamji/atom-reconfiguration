#pragma once

#define NO_RIGHT_PARTNER -1
#define NO_CHAIN_START -1

#include "../../interval/interval.h"

struct AlternatingChains {
  int *right_partners;
  int *chain_start_indexes;
};

struct ChainRange {
  int min_chain;
  int max_chain_exclusive;
};

void alternating_chains_free(struct AlternatingChains *chains);

void alternating_chains_compute_range(const struct Interval *interval,
                                      struct AlternatingChains *chains,
                                      struct ChainRange range);
struct AlternatingChains *
alternating_chains_get(const struct Interval *interval, int imbalance);

int alternating_chains_get_exclusion(const struct AlternatingChains *chains,
                                     int chain_index, int max_exclusion_index);

int *alternating_chains_get_exclusion_from_range(
    const struct AlternatingChains *chains, struct ChainRange range,
    int interval_size);

bool *
alternating_chains_get_exclusion_array(const struct AlternatingChains *chains,
                                       int interval_size, int imbalance);
