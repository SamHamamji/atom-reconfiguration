#include <stdlib.h>

#include "../../interval/interval.h"
#include "../common/alternating_chains.h"
#include "../common/solve_neutral_interval.h"
#include "../linear_solver.h"

struct Mapping *linear_solve_aggarwal(const struct Interval *interval,
                                      const void *params) {
  struct Counts counts = interval_get_counts(interval);
  int imbalance = counts_get_imbalance(counts);
  if (imbalance < 0) {
    return NULL;
  }

  struct AlternatingChains *chains =
      alternating_chains_get(interval, imbalance);

  bool *exclusion_array = alternating_chains_get_exclusion_array(
      chains, interval->length, imbalance);
  alternating_chains_free(chains);

  struct Mapping *mapping =
      solve_neutral_interval(interval, exclusion_array, counts.target_num);
  free(exclusion_array);

  return mapping;
}
