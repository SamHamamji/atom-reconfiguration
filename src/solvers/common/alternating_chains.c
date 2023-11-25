#include "alternating_chains.h"

#include <stdlib.h>

void alternating_chains_free(struct AlternatingChains *chains) {
  free(chains->chain_start_indexes);
  free(chains->right_partners);
  free(chains);
}

struct AlternatingChains *
get_alternating_chains(const struct Interval *const interval, int imbalance) {
  int current_chain = -1;

  int *right_partners = malloc(interval->size * sizeof(int));
  int *chain_start_indexes = malloc(imbalance * sizeof(int));
  int *current_chain_node = malloc(imbalance * sizeof(int));

  for (int i = 0; i < interval->size; i++) {
    right_partners[i] = NO_RIGHT_PARTNER;
  }
  for (int i = 0; i < imbalance; i++) {
    chain_start_indexes[i] = NO_CHAIN_START;
  }

  for (int i = 0; i < interval->size; i++) {
    current_chain = current_chain + (int)interval->array[i].is_source -
                    ((i != 0) ? (int)interval->array[i - 1].is_target : 0);

    if (interval->array[i].is_source == interval->array[i].is_target ||
        current_chain < 0 || current_chain >= imbalance) {
      continue;
    }

    if (chain_start_indexes[current_chain] == NO_CHAIN_START) {
      chain_start_indexes[current_chain] = i;
    } else if (interval->array[i].is_source) {
      right_partners[current_chain_node[current_chain]] = i;
    }
    if (interval->array[i].is_target) {
      right_partners[current_chain_node[current_chain]] = i;
    }
    current_chain_node[current_chain] = i;
  }

  free(current_chain_node);

  struct AlternatingChains *alternating_chains =
      malloc(sizeof(struct AlternatingChains));
  alternating_chains->right_partners = right_partners;
  alternating_chains->chain_start_indexes = chain_start_indexes;
  alternating_chains->interval_size = interval->size;

  return alternating_chains;
}

int get_exclusion_from_chain(const struct AlternatingChains *chains,
                             int chain_index, int max_index) {
  int current_exclusion_cost = 0;
  int current_exclusion_index = chains->chain_start_indexes[chain_index];

  int best_exclusion_cost = current_exclusion_cost;
  int best_exclusion_index = current_exclusion_index;

  while (chains->right_partners[current_exclusion_index] != NO_RIGHT_PARTNER &&
         current_exclusion_index < max_index) {
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
