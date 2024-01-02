#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../grid/grid.h"
#include "../linear_solver/linear_solver.h"
#include "./red-rec.h"
#include "reconfiguration.h"

static void print_array(int *array, int length) {
  for (int i = 0; i < length; i++) {
    printf("%d ", array[i]);
  }
  printf("\n");
}

struct ColumnPair {
  int donor_index;
  int donor_surplus;
  int receiver_index;
  int receiver_deficit;
};

inline static int min(int a, int b) { return a < b ? a : b; }

// Returns a positive number if the first element is greater than the second,
// and vice versa
static int compare_column_pairs(struct ColumnPair a, struct ColumnPair b) {
  /**
   * Comparisons by priority
   * 1. maximizes the number of exchanged tokens
   * 2. minimizes the number of columns between the donor and the receiver
   * 3. maximizes the receiver surplus (closest to saturation)
   */
  int exchanged_sources_a = min(a.donor_surplus, a.receiver_deficit);
  int exchanged_sources_b = min(b.donor_surplus, b.receiver_deficit);
  if (exchanged_sources_a != exchanged_sources_b) {
    return exchanged_sources_a - exchanged_sources_b;
  }

  int column_distance_a = abs(a.donor_index - a.receiver_index);
  int column_distance_b = abs(b.donor_index - b.receiver_index);
  if (column_distance_a != column_distance_b) {
    return column_distance_b - column_distance_a;
  }

  return b.receiver_deficit - a.receiver_deficit;
}

// Returns the best column pair, ensuring that the pair only contains solved
// columns in between the donor and the receiver
static struct ColumnPair get_best_column_pair(struct Grid *grid,
                                              int *column_surpluses,
                                              bool *column_is_solved) {
  int previous_column = -1;
  struct ColumnPair best_column_pair = {
      .receiver_index = -1,
      .donor_index = -1,
      .donor_surplus = 0,
      .receiver_deficit = 0,
  };

  for (int current_column = 0; current_column < grid->width; current_column++) {
    if (column_is_solved[current_column] ||
        column_surpluses[current_column] == 0) {
      continue;
    }
    bool previous_column_is_receiver = column_surpluses[previous_column] < 0;
    bool current_column_is_receiver = column_surpluses[current_column] < 0;
    if (previous_column != -1 &&
        previous_column_is_receiver != current_column_is_receiver) {
      struct ColumnPair current_column_pair;
      if (previous_column_is_receiver) {
        current_column_pair = (struct ColumnPair){
            .donor_index = current_column,
            .donor_surplus = column_surpluses[current_column],
            .receiver_index = previous_column,
            .receiver_deficit = abs(column_surpluses[previous_column]),
        };
      } else {
        current_column_pair = (struct ColumnPair){
            .donor_index = previous_column,
            .donor_surplus = column_surpluses[previous_column],
            .receiver_index = current_column,
            .receiver_deficit = abs(column_surpluses[current_column]),
        };
      }
      if (compare_column_pairs(current_column_pair, best_column_pair) > 0) {
        best_column_pair = current_column_pair;
      }
    }
    previous_column = current_column;
  }
  return best_column_pair;
}

static void solve_column(struct Grid *grid, int column_index,
                         struct Reconfiguration *reconfiguration,
                         bool *column_is_solved) {
  struct Point *column = grid_get_column(grid, column_index);
  struct Mapping *mapping = linear_solve_aggarwal(
      &(const struct Interval){.array = column, .length = grid->height}, NULL);
  for (int i = 0; i < mapping->pair_count; i++) {
    reconfiguration_add_move(
        reconfiguration,
        (struct Move){
            .origin = {.col = column_index, .row = mapping->pairs[i].source},
            .destination = {.col = column_index,
                            .row = mapping->pairs[i].target},
        });
  }
  mapping_free(mapping);
  column_is_solved[column_index] = true;
}

static void solve_neutral_columns(struct Grid *grid, int *column_supluses,
                                  struct Reconfiguration *reconfiguration,
                                  bool *column_is_solved) {
  for (int column_index = 0; column_index < grid->width; column_index++) {
    if (column_supluses[column_index] == 0) {
      solve_column(grid, column_index, reconfiguration, column_is_solved);
    }
  }
}

struct Reconfiguration *red_rec(struct Grid *grid) {
  struct Reconfiguration *reconfiguration =
      malloc(sizeof(struct Reconfiguration));
  *reconfiguration = (struct Reconfiguration){
      .moves = malloc(2 * grid->width * grid->height * sizeof(struct Move)),
      .move_count = 0,
  };

  int *column_supluses = grid_get_column_supluses(grid);
  bool *column_is_solved = calloc(grid->width, sizeof(bool));

  solve_neutral_columns(grid, column_supluses, reconfiguration,
                        column_is_solved);

  struct ColumnPair best_column_pair =
      get_best_column_pair(grid, column_supluses, column_is_solved);
  while (best_column_pair.receiver_index != -1 &&
         best_column_pair.donor_index != -1) {
    printf("Matching %d: %d WITH %d: %d\n", best_column_pair.donor_index,
           best_column_pair.donor_surplus, best_column_pair.receiver_index,
           best_column_pair.receiver_deficit);
    if (best_column_pair.donor_surplus > best_column_pair.receiver_deficit) {
    }
  }

  printf("Column supluses: ");
  print_array(column_supluses, grid->width);

  free(column_supluses);
  free(column_is_solved);
  return reconfiguration;
}

// Require:
// 1:   Compute the surplus for each column Ci, 0 ≤ i < W, where surplus
//      is σi = |S ∩ V (Ci)| − |T ∩ V (Ci)|;
// 2:   Solve columns with a surplus of 0 using the exact 1D algorithm (mean
//      save the moves required, and execute them to modify the input instance);
// 3:   while exists column with negative surplus do
// 4:     Let (Ci, Cj) denote the best donor-receiver pair;
// 5:     Let Ci be the receiver column (negative surplus);
// 6:     Let Cj be the donor column for Ci;
// 7:     if σj − σi > 0 then
// 8:       Reconfigure σi tokens from Cj to Ci;
// 9:       Solve the receiver column Ci (exact 1D);
// 10:    else if σj − σi = 0 then
// 11:      Reconfigure tokens from Cj to Ci;
// 12:      Solve the receiver and donor columns (exact 1D);
// 13:    else
// 14:      Solve the donor column Cj (exact 1D);
// 15:      Mark excess tokens in Cj for delayed moving to Ci;
// 16:    end if
// 17:    Update surpluses and pairs;
// 18:    Write delayed moves if/when possible;
// 19:  end while
// 20:  Solve remaining (unsolved) columns with non-negative surplus using the
//      exact 1D algorithm;
