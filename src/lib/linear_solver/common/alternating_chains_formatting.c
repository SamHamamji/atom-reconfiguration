#include <stdio.h>

#include "./alternating_chains.h"

void alternating_chains_print(const struct AlternatingChains *chains) {
  if (chains->chain_num == 0) {
    printf("Empty alternating chains\n");
    return;
  }
  for (int i = 0; i < chains->chain_num; i++) {
    printf("%d: ", i);
    int current = chains->chain_start_indexes[i];
    do {
      printf("%d ", current);
      current = chains->right_partners[current];
    } while (current != NO_RIGHT_PARTNER);

    printf("\n");
  }
}
