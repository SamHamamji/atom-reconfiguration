#pragma once

#include <stdbool.h>

#include "../grid/grid.h"

struct ColumnPair {
  int donor_index;
  int receiver_index;
};

/** Returns the best column pair
 * Makes sure that the pair only contains solved columns in between the donor
 * and the receiver
 */
struct ColumnPair column_pair_get_best(struct Grid *grid,
                                       struct Counts *column_counts,
                                       bool *column_is_solved);
