#include <stdlib.h>

#include "../interval/interval.h"
#include "common/alternating_chains.h"
#include "common/solve_neutral_interval.h"
#include "solver.h"

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
