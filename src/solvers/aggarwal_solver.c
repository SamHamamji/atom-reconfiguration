#include <stdlib.h>

#include "../interval/interval.h"
#include "common/alternating_chains.h"
#include "common/solve_neutral_interval.h"
#include "common/stack.h"
#include "solver.h"

static int get_initial_exclusion_cost(const struct AlternatingChains *chains,
                                      int chain_start_index) {
  int initial_exclusion_cost = -chain_start_index;
  for (int i = chain_start_index;;
       i = chains->target_right_partners[chains->source_right_partners[i]]) {
    if (chains->source_right_partners[i] == NO_RIGHT_PARTNER) {
      initial_exclusion_cost += i;
      break;
    }
    initial_exclusion_cost += i - chains->source_right_partners[i];
  }
  return initial_exclusion_cost;
}

static int get_exclusion_from_chain(const struct AlternatingChains *chains,
                                    int chain_start_index) {
  int best_exclusion_index = chain_start_index;
  int best_exclusion_cost = 0;

  int current_exclusion_cost = best_exclusion_cost;
  int current_exclusion_index = chain_start_index;

  while (chains->source_right_partners[current_exclusion_index] !=
         NO_RIGHT_PARTNER) {
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

static bool *get_exclusion_from_chains(const struct Interval *const interval,
                                       const struct AlternatingChains *chains,
                                       int imbalance) {
  int *excluded_indexes = malloc(sizeof(int) * imbalance);

  for (int chain_index = 0; chain_index < imbalance; chain_index++) {
    excluded_indexes[chain_index] = get_exclusion_from_chain(
        chains, chains->chain_start_indexes[chain_index]);
  }

  bool *exclusion_array = calloc(sizeof(bool), interval->size);
  for (int i = 0; i < imbalance; i++) {
    exclusion_array[excluded_indexes[i]] = true;
  }
  free(excluded_indexes);

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

  bool *exclusion_array =
      get_exclusion_from_chains(interval, chains, imbalance);

  alternating_chains_free(chains);

  struct Mapping *mapping = solve_neutral_interval(interval, exclusion_array);

  free(exclusion_array);

  return mapping;
}

const struct Solver aggarwal_solver = {
    .solve = solver_function,
    .name = "Aggarwal solver",
};
