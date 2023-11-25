#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "../interval/interval.h"
#include "common/alternating_chains.h"
#include "common/solve_neutral_interval.h"
#include "common/stack.h"
#include "solver.h"

#define THREAD_NUMBER 8

struct ThreadInputContext {
  const struct AlternatingChains *chains;
  int *output;
};

struct ThreadInput {
  struct ThreadInputContext context;
  struct {
    int min_chain;
    int max_chain;
  } chain_range;
};

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

static void *get_exclusion_from_chain_range(void *const args) {
  const struct ThreadInput *const input = (struct ThreadInput *)args;
  const int output_length =
      input->chain_range.max_chain - input->chain_range.min_chain;
  int *excluded_indexes = malloc(output_length * sizeof(int));

  int max_index = input->context.chains->interval_size - 1;

  for (int height = input->chain_range.max_chain - 1;
       height >= input->chain_range.min_chain; height--) {
    excluded_indexes[height - input->chain_range.min_chain] =
        get_exclusion_from_chain(input->context.chains, height, max_index);
  }

  memcpy(&input->context.output[input->chain_range.min_chain], excluded_indexes,
         output_length * sizeof(int));

  free(excluded_indexes);
  pthread_exit(NULL);
}

static bool *get_exclusion_from_chains(const struct AlternatingChains *chains,
                                       int imbalance) {
  int *excluded_indexes = malloc(sizeof(int) * imbalance);
  pthread_t *thread_array = malloc(THREAD_NUMBER * sizeof(pthread_t));
  struct ThreadInput *thread_inputs =
      malloc(THREAD_NUMBER * sizeof(struct ThreadInput));

  const int heights_per_thread = imbalance / THREAD_NUMBER;
  const int remaining_heights = imbalance % THREAD_NUMBER;
  const struct ThreadInputContext context = {
      .chains = chains,
      .output = excluded_indexes,
  };

  for (int i = 0; i < THREAD_NUMBER; i++) {
    thread_inputs[i].chain_range.min_chain =
        (i == 0 ? 0 : thread_inputs[i - 1].chain_range.max_chain);
    thread_inputs[i].chain_range.max_chain =
        thread_inputs[i].chain_range.min_chain + heights_per_thread +
        (int)(i < remaining_heights);
    thread_inputs[i].context = context;

    pthread_create(&thread_array[i], NULL, get_exclusion_from_chain_range,
                   &thread_inputs[i]);
  }

  bool *exclusion_array = calloc(chains->interval_size, sizeof(bool));
  for (int i = 0; i < THREAD_NUMBER; i++) {
    pthread_join(thread_array[i], NULL);
    for (int height = thread_inputs[i].chain_range.min_chain;
         height < thread_inputs[i].chain_range.max_chain; height++) {
      exclusion_array[excluded_indexes[height]] = true;
    }
  }

  free(thread_inputs);
  free(thread_array);
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

  bool *exclusion_array = get_exclusion_from_chains(chains, imbalance);
  alternating_chains_free(chains);

  struct Mapping *mapping = solve_neutral_interval(interval, exclusion_array);
  free(exclusion_array);

  return mapping;
}

const struct Solver aggarwal_parallel_solver = {
    .solve = solver_function,
    .name = "Aggarwal solver parallel",
};
