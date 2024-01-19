#include <stdio.h>

#include "./alternating_chains.h"

void alternating_chains_print(const struct AlternatingChains *chains,
                              int imbalance) {
  for (int i = 0; i < imbalance; i++) {
    printf("%d: ", i);
    int current = chains->chain_start_indexes[i];
    do {
      printf("%d ", current);
      current = chains->right_partners[current];
    } while (current != NO_RIGHT_PARTNER);

    printf("\n");
  }
}
