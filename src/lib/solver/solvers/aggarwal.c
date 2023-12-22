#include <stdlib.h>

#include "../../interval/interval.h"
#include "../common/alternating_chains.h"
#include "../common/solve_neutral_interval.h"
#include "../solver.h"

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
      alternating_chains_get(interval, imbalance);

  bool *exclusion_array =
      alternating_chains_get_exclusion_array(chains, interval->size, imbalance);
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
