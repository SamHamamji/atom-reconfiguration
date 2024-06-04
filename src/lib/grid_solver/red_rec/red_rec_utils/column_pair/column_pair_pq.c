#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../../../utils/colors.h"
#include "../../../../utils/max_min.h"
#include "../red_rec_utils.h"

#define ROOT 0
#define NODES_TO_FIX_NUM 2

inline static int get_parent(int node) { return (node - 1) / 2; }
inline static int get_left_child(int node) { return 2 * node + 1; }
inline static int get_right_child(int node) { return 2 * node + 2; }

inline static bool node_exists(int node, int pq_pair_num) {
  return node >= ROOT && node < pq_pair_num;
}

static bool is_ancestor(int parent, int child) {
  int p = parent + 1;
  int c = child + 1;

  int p_bits = log2(p);
  int c_bits = log2(c);

  if (p_bits > c_bits) {
    return false;
  }

  return (c >> (c_bits - p_bits)) == p;
}

static int pq_node_get_exchange_num(struct ColumnPairNode pair,
                                    struct Counts *column_counts) {
  int left_imbalance = counts_get_imbalance(column_counts[pair.left_column]);
  int right_imbalance = counts_get_imbalance(column_counts[pair.right_column]);

  if ((left_imbalance >= 0) == (right_imbalance >= 0)) {
    return 0;
  }

  return min(abs(left_imbalance), abs(right_imbalance));
}

/**
 * Returns a positive number if the first node has a higher priority than the
 * second one, and vice versa
 *
 * Compares by priority
 * 1. maximizes the number of exchanged tokens
 * 2. minimizes the number of columns between the donor and the receiver
 * 3. maximizes the receiver surplus (closest receiver to saturation)
 */
static int compare_column_pairs(const struct ColumnPairPQ *pq, int a, int b) {
  struct ColumnPairNode node_a = pq->heap[a];
  struct ColumnPairNode node_b = pq->heap[b];

  int exchange_num_a = pq_node_get_exchange_num(node_a, pq->column_counts);
  int exchange_num_b = pq_node_get_exchange_num(node_b, pq->column_counts);

  if (exchange_num_a != exchange_num_b) {
    return exchange_num_a - exchange_num_b;
  }

  int column_distance_a = abs(node_a.left_column - node_a.right_column);
  int column_distance_b = abs(node_b.left_column - node_b.right_column);

  if (column_distance_a != column_distance_b) {
    return column_distance_b - column_distance_a;
  }

  int receiver_deficit_a =
      -min(counts_get_imbalance(pq->column_counts[node_a.right_column]),
           counts_get_imbalance(pq->column_counts[node_a.left_column]));

  int receiver_deficit_b =
      -min(counts_get_imbalance(pq->column_counts[node_b.right_column]),
           counts_get_imbalance(pq->column_counts[node_b.left_column]));

  return receiver_deficit_b - receiver_deficit_a;
}

static bool column_pair_node_is_valid(const struct ColumnPairPQ *pq, int i) {
  struct ColumnPairNode node = pq->heap[i];

  if (node_exists(node.left_pair_index, pq->pair_num)) {
    if (i != pq->heap[node.left_pair_index].right_pair_index) {
      printf(RED "Left pointer mismatch at index %d\n" RESET, i);
      column_pair_pq_print(pq);
      return false;
    }

    if (node.left_column != pq->heap[node.left_pair_index].right_column) {
      printf(RED "Left column mismatch at index %d\n" RESET, i);
      column_pair_pq_print(pq);
      return false;
    }
  }

  if (node_exists(node.right_pair_index, pq->pair_num)) {
    if (i != pq->heap[node.right_pair_index].left_pair_index) {
      printf(RED "Right pointer mismatch at index %d\n" RESET, i);
      column_pair_pq_print(pq);
      return false;
    }

    if (node.right_column != pq->heap[node.right_pair_index].left_column) {
      printf(RED "Right column mismatch at index %d\n" RESET, i);
      column_pair_pq_print(pq);
      return false;
    }
  }

  return true;
}

static bool column_pair_pq_is_valid(const struct ColumnPairPQ *pq) {
  for (int i = 0; i < pq->pair_num; i++) {
    if (!column_pair_node_is_valid(pq, i)) {
      return false;
    }

    if (i != 0 && compare_column_pairs(pq, get_parent(i), i) < 0) {
      printf(RED "Order mismatch between parent %d and child %d\n" RESET,
             get_parent(i), i);
      column_pair_pq_print(pq);
      return false;
    }
  }

  return true;
}

static void column_pair_pq_swap(struct ColumnPairPQ *pq, int i, int j) {
  struct ColumnPairNode a = pq->heap[i];
  struct ColumnPairNode b = pq->heap[j];

  if (a.left_pair_index == j) {
    a.left_pair_index = i;
    b.right_pair_index = j;
  } else {
    if (node_exists(a.left_pair_index, pq->pair_num)) {
      pq->heap[a.left_pair_index].right_pair_index = j;
    }
    if (node_exists(b.right_pair_index, pq->pair_num)) {
      pq->heap[b.right_pair_index].left_pair_index = i;
    }
  }

  if (a.right_pair_index == j) {
    a.right_pair_index = i;
    b.left_pair_index = j;
  } else {
    if (node_exists(a.right_pair_index, pq->pair_num)) {
      pq->heap[a.right_pair_index].left_pair_index = j;
    }
    if (node_exists(b.left_pair_index, pq->pair_num)) {
      pq->heap[b.left_pair_index].right_pair_index = i;
    }
  }

  pq->heap[j] = a;
  pq->heap[i] = b;
}

/** Returns the new index of the node */
static int column_pair_pq_bubble_down(struct ColumnPairPQ *pq, int index) {
  assert(index >= 0);
  assert(index < pq->pair_num);

  int current = index;
  int left_child = get_left_child(current);
  int right_child = get_right_child(current);
  while (right_child < pq->pair_num) {
    assert(left_child < pq->pair_num);
    assert(right_child < pq->pair_num);
    int smaller_child = compare_column_pairs(pq, left_child, right_child) > 0
                            ? left_child
                            : right_child;
    if (compare_column_pairs(pq, current, smaller_child) < 0) {
      column_pair_pq_swap(pq, current, smaller_child);
    } else {
      return current;
    }

    current = smaller_child;
    left_child = get_left_child(current);
    right_child = get_right_child(current);
  }

  if (right_child == pq->pair_num &&
      compare_column_pairs(pq, current, left_child) < 0) {
    column_pair_pq_swap(pq, current, left_child);
    current = left_child;
  }

  return current;
}

/** Returns the new index of the node */
static int column_pair_pq_bubble_up(struct ColumnPairPQ *pq, int index) {
  assert(index >= 0);
  assert(index < pq->pair_num);

  int current = index;
  int parent = get_parent(current);
  while (current > ROOT && compare_column_pairs(pq, current, parent) > 0) {
    column_pair_pq_swap(pq, current, parent);
    current = parent;
    parent = get_parent(current);
  }

  return current;
}

struct ColumnPairPQ column_pair_pq_new(struct Counts *column_counts,
                                       int grid_width) {
  (void)column_pair_pq_is_valid; // to avoid unused function warning

  struct ColumnPairPQ pq = {
      .heap = malloc(grid_width * sizeof(struct ColumnPairNode)),
      .column_counts = column_counts,
      .pair_num = 0,
      .grid_width = grid_width,
  };

  int previous_column = 0;
  for (int current_column = 1; current_column < grid_width; current_column++) {
    int current_imbalance = counts_get_imbalance(column_counts[current_column]);
    if (current_imbalance == 0) {
      continue;
    }

    pq.heap[pq.pair_num] = (struct ColumnPairNode){
        .left_column = previous_column,
        .right_column = current_column,
        .left_pair_index = pq.pair_num - 1,
        .right_pair_index = pq.pair_num + 1,
    };
    pq.pair_num++;

    previous_column = current_column;
  }

  for (int i = pq.pair_num - 1; i >= ROOT; i--) {
    column_pair_pq_bubble_down(&pq, i);
  }

  assert(column_pair_pq_is_valid(&pq));

  return pq;
}

bool column_pair_pq_is_empty(const struct ColumnPairPQ *pq) {
  if (pq->pair_num == 0) {
    return true;
  }

  return pq_node_get_exchange_num(pq->heap[ROOT], pq->column_counts) == 0;
}

static int get_updated_node_index(int node, int other_source, int other_target,
                                  int pq_pair_num) {
  if (node == pq_pair_num || node == other_source) {
    return other_target;
  }

  if (is_ancestor(other_source, node) && is_ancestor(node, other_target)) {
    return get_parent(node);
  }

  if (is_ancestor(other_target, node) && is_ancestor(node, other_source)) {
    int left = get_left_child(node);
    int right = get_right_child(node);
    return is_ancestor(right, other_source) ? right : left;
  };

  return node;
}

static void column_pair_delete_root_neighbor(struct ColumnPairPQ *pq,
                                             bool delete_left) {
  int neighbor = delete_left ? pq->heap[ROOT].left_pair_index
                             : pq->heap[ROOT].right_pair_index;

  int right_node = pq->heap[neighbor].right_pair_index;
  int left_node = pq->heap[neighbor].left_pair_index;

  if (node_exists(right_node, pq->pair_num)) {
    pq->heap[right_node].left_pair_index = left_node;
    if (delete_left) {
      pq->heap[right_node].left_column = pq->heap[neighbor].left_column;
    }
  }

  if (node_exists(left_node, pq->pair_num)) {
    pq->heap[left_node].right_pair_index = right_node;
    if (!delete_left) {
      pq->heap[left_node].right_column = pq->heap[neighbor].right_column;
    }
  }

  assert(column_pair_node_is_valid(pq, ROOT));
  assert(!node_exists(delete_left ? left_node : right_node, pq->pair_num) ||
         column_pair_node_is_valid(pq, delete_left ? left_node : right_node));

  pq->heap[neighbor].left_pair_index = -1;
  pq->heap[neighbor].right_pair_index = -1;

  column_pair_pq_swap(pq, neighbor, pq->pair_num - 1);
  pq->pair_num--;
}

static void column_pair_fix_neighbor(struct ColumnPairPQ *pq, int *nodes_to_fix,
                                     int neighbor) {
  assert(neighbor == pq->heap[ROOT].left_pair_index ||
         neighbor == pq->heap[ROOT].right_pair_index);

  const bool fix_left = neighbor == pq->heap[ROOT].left_pair_index;

  assert(!node_exists(neighbor, pq->pair_num) ||
         pq->heap[fix_left ? neighbor : ROOT].right_column ==
             pq->heap[fix_left ? ROOT : neighbor].left_column);

  int common_column =
      fix_left ? pq->heap[ROOT].left_column : pq->heap[ROOT].right_column;

  if (counts_get_imbalance(pq->column_counts[common_column]) == 0) {
    column_pair_delete_root_neighbor(pq, fix_left);
  }

  if (neighbor != pq->pair_num) {
    int new_index = column_pair_pq_bubble_down(pq, neighbor);

    for (int i = 0; i < NODES_TO_FIX_NUM; i++) {
      nodes_to_fix[i] = get_updated_node_index(nodes_to_fix[i], neighbor,
                                               new_index, pq->pair_num);
    }
  }
}

static struct ColumnPair column_node_to_pair(struct ColumnPairPQ *pq,
                                             int index) {
  int left_imbalance =
      counts_get_imbalance(pq->column_counts[pq->heap[index].left_column]);
  int right_imbalance =
      counts_get_imbalance(pq->column_counts[pq->heap[index].right_column]);

  return left_imbalance > 0
             ? ((struct ColumnPair){
                   .donor_index = pq->heap[index].left_column,
                   .receiver_index = pq->heap[index].right_column,
                   .donor_surplus = left_imbalance,
                   .receiver_deficit = right_imbalance,
               })
             : ((struct ColumnPair){
                   .donor_index = pq->heap[index].right_column,
                   .receiver_index = pq->heap[index].left_column,
                   .donor_surplus = right_imbalance,
                   .receiver_deficit = left_imbalance,
               });
}

static void nodes_to_fix_sort(int *nodes_to_fix) {
  if (nodes_to_fix[0] > nodes_to_fix[1]) {
    int temp = nodes_to_fix[0];
    nodes_to_fix[0] = nodes_to_fix[1];
    nodes_to_fix[1] = temp;
  }
}

struct ColumnPair column_pair_pq_pop(struct ColumnPairPQ *pq) {
  assert(pq_node_get_exchange_num(pq->heap[ROOT], pq->column_counts) != 0);

  struct ColumnPair best_pair = column_node_to_pair(pq, ROOT);

  int exchange_num =
      pq_node_get_exchange_num(pq->heap[ROOT], pq->column_counts);
  pq->column_counts[best_pair.donor_index].source_num -= exchange_num;
  pq->column_counts[best_pair.receiver_index].source_num += exchange_num;

  int nodes_to_fix[NODES_TO_FIX_NUM] = {
      pq->heap[ROOT].left_pair_index,
      pq->heap[ROOT].right_pair_index,
  };

  nodes_to_fix_sort(nodes_to_fix);

  for (int i = NODES_TO_FIX_NUM - 1; i >= 0; i--) {
    if (node_exists(nodes_to_fix[i], pq->pair_num)) {
      column_pair_fix_neighbor(pq, nodes_to_fix, nodes_to_fix[i]);
    }
  }

  int new_root_index = column_pair_pq_bubble_down(pq, ROOT);

  for (int i = 0; i < NODES_TO_FIX_NUM; i++) {
    nodes_to_fix[i] = get_updated_node_index(nodes_to_fix[i], ROOT,
                                             new_root_index, pq->pair_num);
  }

  nodes_to_fix_sort(nodes_to_fix);

  for (int i = 0; i < NODES_TO_FIX_NUM; i++) {
    if (node_exists(nodes_to_fix[i], pq->pair_num)) {
      column_pair_pq_bubble_up(pq, nodes_to_fix[i]);
    }
  }

  assert(column_pair_pq_is_valid(pq));

  return best_pair;
}

void column_pair_pq_free(struct ColumnPairPQ *pq) { free(pq->heap); }
