#include <stdlib.h>

#include "../../interval/interval.h"
#include "../common/alternating_chains.h"
#include "../common/solve_neutral_interval.h"
#include "../solver.h"

static bool *get_exclusion_from_chains(const struct AlternatingChains *chains,
                                       int interval_size, int imbalance) {
  bool *exclusion_array = calloc(sizeof(bool), interval_size);
  int max_exclusion_index = interval_size;

  for (int chain_index = imbalance - 1; chain_index >= 0; chain_index--) {
    int excluded =
        get_exclusion_from_chain(chains, chain_index, max_exclusion_index);
    exclusion_array[excluded] = true;
    max_exclusion_index = excluded;
  }

  return exclusion_array;
}

static struct Mapping *solver_function(const struct Interval *interval) {
  if (interval->size <= 0) {
    return mapping_get_null();
  }

  struct IntervalCounts counts = interval_get_counts(interval);
  int imbalance = get_imbalance_from_counts(counts);
  if (imbalance < 0) {
    return mapping_get_null();
  }

  struct AlternatingChains *chains =
      get_alternating_chains(interval, imbalance);

  bool *exclusion_array =
      get_exclusion_from_chains(chains, interval->size, imbalance);
  alternating_chains_free(chains);

  struct Mapping *mapping =
      solve_neutral_interval(interval, exclusion_array, counts.target_num);
  free(exclusion_array);

  return mapping;
}

const struct Solver aggarwal_solver = {
    .solve = solver_function,
    .name = "Aggarwal solver",
};
