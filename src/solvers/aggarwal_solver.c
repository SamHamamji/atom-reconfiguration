#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../interval/interval.h"
#include "common/solve_neutral_interval.h"
#include "common/stack.h"
#include "solver.h"

#define NO_PARTNER -1

struct AlternatingChains {
  int *source_right_partners;
  int *target_right_partners;
  int *chain_start_indexes;
};

static void alternating_chains_free(struct AlternatingChains *chains) {
  free(chains->chain_start_indexes);
  free(chains->source_right_partners);
  free(chains->target_right_partners);
  free(chains);
}

static struct AlternatingChains *
get_alternating_chains(const struct Interval *const interval, int imbalance) {
  int *source_right_partners = malloc(interval->size * sizeof(int));
  int *target_right_partners = malloc(interval->size * sizeof(int));
  int *chain_start_indexes = malloc(imbalance * sizeof(int));

  memset(target_right_partners, NO_PARTNER, interval->size * sizeof(int));
  memset(source_right_partners, NO_PARTNER, interval->size * sizeof(int));

  struct StackElement *target_index_stack_head = NULL;
  struct StackElement *source_index_stack_head = NULL;

  int chain_counter = 0;
  for (int i = 0; i < interval->size; i++) {
    if (interval->array[i].is_source) {
      source_index_stack_head = stack_push(source_index_stack_head, i);
      if (!stack_is_empty(target_index_stack_head)) {
        target_right_partners[target_index_stack_head->value] = i;

        target_index_stack_head = stack_pop(target_index_stack_head);
      } else if (chain_counter < imbalance) {
        chain_start_indexes[chain_counter] = i;
        chain_counter++;
      }
    }
    if (interval->array[i].is_target) {
      target_index_stack_head = stack_push(target_index_stack_head, i);
      if (!stack_is_empty(source_index_stack_head)) {
        source_right_partners[source_index_stack_head->value] = i;
        source_index_stack_head = stack_pop(source_index_stack_head);
      }
    }
  }

  stack_free(target_index_stack_head);
  stack_free(source_index_stack_head);

  struct AlternatingChains *alternating_chains =
      malloc(sizeof(struct AlternatingChains));
  alternating_chains->target_right_partners = target_right_partners;
  alternating_chains->source_right_partners = source_right_partners;
  alternating_chains->chain_start_indexes = chain_start_indexes;
  return alternating_chains;
}

static int get_initial_exclusion_cost(const struct AlternatingChains *chains,
                                      int chain_start_index) {
  int initial_exclusion_cost = -chain_start_index;
  for (int i = chain_start_index;;
       i = chains->target_right_partners[chains->source_right_partners[i]]) {
    if (chains->source_right_partners[i] == NO_PARTNER) {
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
  int best_exclusion_cost =
      get_initial_exclusion_cost(chains, chain_start_index);

  int current_exclusion_cost = best_exclusion_cost;
  int current_exclusion_index = chain_start_index;

  while (true) {
    if (chains->source_right_partners[current_exclusion_index] == NO_PARTNER) {
      break;
    }
    current_exclusion_cost =
        current_exclusion_cost - current_exclusion_index +
        chains->source_right_partners[current_exclusion_index] * 2 -
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
