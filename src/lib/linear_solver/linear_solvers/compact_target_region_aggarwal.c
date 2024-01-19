#include <assert.h>
#include <stdlib.h>

#include "../../interval/interval.h"
#include "../common/alternating_chains.h"
#include "../common/solve_neutral_interval.h"
#include "../linear_solver.h"

static bool *alternating_chains_get_exclusion_array_from_compact_target_region(
    const struct AlternatingChains *chains, int interval_length,
    int imbalance) {
  bool *exclusion_array = calloc(sizeof(bool), interval_length);

  struct Range search_range = {0, imbalance};

  while (
      chains->right_partners
              [chains->chain_start_indexes[search_range.exclusive_end - 1]] ==
          NO_RIGHT_PARTNER &&
      search_range.exclusive_end > search_range.start) {
    search_range.exclusive_end--;
  }

  while (search_range.start != search_range.exclusive_end) {
    int mid = (search_range.start + search_range.exclusive_end) / 2;
    int excluded =
        alternating_chains_get_exclusion(chains, mid, interval_length);
    bool first_source_is_excluded =
        chains->chain_start_indexes[mid] == excluded;

    if (first_source_is_excluded) {
      for (int i = search_range.start; i <= mid; i++) {
        exclusion_array[chains->chain_start_indexes[i]] = true;
      }
      search_range.start = mid + 1;
    } else {
      for (int i = mid; i < search_range.exclusive_end; i++) {
        exclusion_array
            [chains->right_partners
                 [chains->right_partners[chains->chain_start_indexes[i]]]] =
                true;
      }
      search_range.exclusive_end = mid;
    }
  }

  int max_exclusion_index = interval_length;

  for (int chain_index = imbalance - 1; chain_index >= 0; chain_index--) {
    int excluded = alternating_chains_get_exclusion(chains, chain_index,
                                                    max_exclusion_index);
    exclusion_array[excluded] = true;
    max_exclusion_index = excluded;
  }

  return exclusion_array;
}

struct Mapping *
linear_solve_compact_target_region_aggarwal(const struct Interval *interval,
                                            const void *params) {
  assert(params == NULL);
  assert(interval_target_region_is_compact(interval));

  struct Counts counts = interval_get_counts(interval);
  int imbalance = counts_get_imbalance(counts);
  if (imbalance < 0) {
    return NULL;
  }

  struct AlternatingChains *chains =
      alternating_chains_get(interval, imbalance);

  bool *exclusion_array =
      alternating_chains_get_exclusion_array_from_compact_target_region(
          chains, interval->length, imbalance);

  alternating_chains_free(chains);

  struct Mapping *mapping =
      solve_neutral_interval(interval, exclusion_array, counts.target_num);
  free(exclusion_array);

  return mapping;
}
