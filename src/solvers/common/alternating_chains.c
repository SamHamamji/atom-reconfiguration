#include "alternating_chains.h"

#include <stdlib.h>

void alternating_chains_free(struct AlternatingChains *chains) {
  free(chains->chain_start_indexes);
  free(chains->source_right_partners);
  free(chains->target_right_partners);
  free(chains);
}

struct AlternatingChains *
get_alternating_chains(const struct Interval *const interval, int imbalance) {
  int current_chain = -1;

  int *source_right_partners = malloc(interval->size * sizeof(int));
  int *target_right_partners = malloc(interval->size * sizeof(int));
  int *chain_start_indexes = malloc(imbalance * sizeof(int));
  int *current_chain_node = malloc(imbalance * sizeof(int));

  for (int i = 0; i < interval->size; i++) {
    source_right_partners[i] = NO_RIGHT_PARTNER;
    target_right_partners[i] = NO_RIGHT_PARTNER;
  }
  for (int i = 0; i < imbalance; i++) {
    chain_start_indexes[i] = NO_CHAIN_START;
  }

  for (int i = 0; i < interval->size; i++) {
    current_chain = current_chain + (int)interval->array[i].is_source -
                    ((i != 0) ? (int)interval->array[i - 1].is_target : 0);

    if (!(interval->array[i].is_source || interval->array[i].is_target) ||
        current_chain < 0 || current_chain >= imbalance) {
      continue;
    }

    if (chain_start_indexes[current_chain] == NO_CHAIN_START) {
      chain_start_indexes[current_chain] = i;
      current_chain_node[current_chain] = i;
    } else if (interval->array[i].is_source) {
      target_right_partners[current_chain_node[current_chain]] = i;
      current_chain_node[current_chain] = i;
    }
    if (interval->array[i].is_target) {
      source_right_partners[current_chain_node[current_chain]] = i;
      current_chain_node[current_chain] = i;
    }
  }

  free(current_chain_node);

  struct AlternatingChains *alternating_chains =
      malloc(sizeof(struct AlternatingChains));
  alternating_chains->target_right_partners = target_right_partners;
  alternating_chains->source_right_partners = source_right_partners;
  alternating_chains->chain_start_indexes = chain_start_indexes;

  return alternating_chains;
}
