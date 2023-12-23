#include "alternating_chains.h"

#include <stdlib.h>

void alternating_chains_free(struct AlternatingChains *chains) {
  free(chains->chain_start_indexes);
  free(chains->right_partners);
  free(chains);
}

struct AlternatingChains *
alternating_chains_get(const struct Interval *interval, int imbalance) {
  struct AlternatingChains *alternating_chains =
      malloc(sizeof(struct AlternatingChains));
  alternating_chains->right_partners = malloc(interval->size * sizeof(int));
  alternating_chains->chain_start_indexes = malloc(imbalance * sizeof(int));

  alternating_chains_compute_range(interval, alternating_chains,
                                   (struct ChainRange){0, imbalance});

  return alternating_chains;
}

void alternating_chains_compute_range(const struct Interval *interval,
                                      struct AlternatingChains *chains,
                                      struct ChainRange range) {
  int chain_range_length = range.max_chain_exclusive - range.min_chain;
  int *current_chain_node = malloc(chain_range_length * sizeof(int));
  int current_chain = -1;

  for (int i = range.min_chain; i < range.max_chain_exclusive; i++) {
    chains->chain_start_indexes[i] = NO_CHAIN_START;
  }

  for (int i = 0; i < interval->size; i++) {
    current_chain = current_chain + (int)interval->array[i].is_source -
                    (int)(i != 0 && interval->array[i - 1].is_target);

    if (interval->array[i].is_source == interval->array[i].is_target ||
        current_chain < range.min_chain ||
        current_chain >= range.max_chain_exclusive) {
      continue;
    }

    if (chains->chain_start_indexes[current_chain] == NO_CHAIN_START) {
      chains->chain_start_indexes[current_chain] = i;
    } else {
      chains->right_partners[current_chain_node[current_chain -
                                                range.min_chain]] = i;
    }
    current_chain_node[current_chain - range.min_chain] = i;
  }

  for (int i = 0; i < chain_range_length; i++) {
    chains->right_partners[current_chain_node[i]] = NO_RIGHT_PARTNER;
  }

  free(current_chain_node);
}

// Gets excluded source index from a chain
// Uses relative minimum cost and early stopping to increase performance
int alternating_chains_get_exclusion(const struct AlternatingChains *chains,
                                     int chain_index, int max_exclusion_index) {
  int current_exclusion_cost = 0;
  int current_exclusion_index = chains->chain_start_indexes[chain_index];

  int best_exclusion_cost = current_exclusion_cost;
  int best_exclusion_index = current_exclusion_index;

  while (chains->right_partners[current_exclusion_index] != NO_RIGHT_PARTNER &&
         current_exclusion_index < max_exclusion_index) {
    current_exclusion_cost +=
        chains->right_partners[current_exclusion_index] * 2 -
        current_exclusion_index -
        chains->right_partners[chains->right_partners[current_exclusion_index]];

    current_exclusion_index =
        chains->right_partners[chains->right_partners[current_exclusion_index]];

    if (current_exclusion_cost <= best_exclusion_cost) {
      best_exclusion_cost = current_exclusion_cost;
      best_exclusion_index = current_exclusion_index;
    }
  }

  return best_exclusion_index;
}

int *alternating_chains_get_exclusion_from_range(
    const struct AlternatingChains *chains, struct ChainRange range,
    int interval_size) {
  const int chain_range_length = range.max_chain_exclusive - range.min_chain;
  int *excluded_indexes = malloc(chain_range_length * sizeof(int));

  int max_exclusion_index = interval_size - 1; // For early stopping
  for (int height = range.max_chain_exclusive - 1; height >= range.min_chain;
       height--) {
    excluded_indexes[height - range.min_chain] =
        alternating_chains_get_exclusion(chains, height, max_exclusion_index);
  }

  return excluded_indexes;
}

bool *
alternating_chains_get_exclusion_array(const struct AlternatingChains *chains,
                                       int interval_size, int imbalance) {
  bool *exclusion_array = calloc(sizeof(bool), interval_size);
  int max_exclusion_index = interval_size;

  for (int chain_index = imbalance - 1; chain_index >= 0; chain_index--) {
    int excluded = alternating_chains_get_exclusion(chains, chain_index,
                                                    max_exclusion_index);
    exclusion_array[excluded] = true;
    max_exclusion_index = excluded;
  }

  return exclusion_array;
}
