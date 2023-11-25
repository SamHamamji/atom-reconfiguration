#include <stdlib.h>

#include "../interval/interval.h"
#include "common/alternating_chains.h"
#include "common/solve_neutral_interval.h"
#include "common/stack.h"
#include "solver.h"

static int get_exclusion_from_chain(const struct AlternatingChains *chains,
                                    int chain_index, int max_index) {
  int current_exclusion_cost = 0;
  int current_exclusion_index = chains->chain_start_indexes[chain_index];

  int best_exclusion_cost = current_exclusion_cost;
  int best_exclusion_index = current_exclusion_index;

  while (chains->source_right_partners[current_exclusion_index] !=
             NO_RIGHT_PARTNER &&
         current_exclusion_index < max_index) {
    current_exclusion_cost +=
        chains->source_right_partners[current_exclusion_index] * 2 -
        current_exclusion_index -
        chains->target_right_partners
            [chains->source_right_partners[current_exclusion_index]];

    current_exclusion_index =
        chains->target_right_partners
            [chains->source_right_partners[current_exclusion_index]];

    if (current_exclusion_cost <= best_exclusion_cost) {
      best_exclusion_cost = current_exclusion_cost;
      best_exclusion_index = current_exclusion_index;
    }
  }

  return best_exclusion_index;
}

static bool *get_exclusion_from_chains(const struct AlternatingChains *chains,
                                       int imbalance) {
  bool *exclusion_array = calloc(sizeof(bool), chains->interval_size);
  int max_exclusion_index = chains->interval_size;

  for (int chain_index = imbalance - 1; chain_index >= 0; chain_index--) {
    int excluded =
        get_exclusion_from_chain(chains, chain_index, max_exclusion_index);
    exclusion_array[excluded] = true;
    max_exclusion_index = excluded;
  }

  return exclusion_array;
}

static struct Mapping *solver_function(const struct Interval *const interval) {
  if (interval->size <= 0) {
    return mapping_get_null();
  }

  int imbalance = interval_get_imbalance(interval);
  if (imbalance < 0) {
    return mapping_get_null();
  }

  struct AlternatingChains *chains =
      get_alternating_chains(interval, imbalance);

  bool *exclusion_array = get_exclusion_from_chains(chains, imbalance);
  alternating_chains_free(chains);

  struct Mapping *mapping = solve_neutral_interval(interval, exclusion_array);
  free(exclusion_array);

  return mapping;
}

const struct Solver aggarwal_solver = {
    .solve = solver_function,
    .name = "Aggarwal solver",
};
