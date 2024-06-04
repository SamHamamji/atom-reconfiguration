#pragma once

#include "../grid/reconfiguration.h"

typedef void GridSolverParams;
typedef struct Reconfiguration *
GridSolverFunction(struct Grid *grid, const GridSolverParams *params);

struct GridSolver {
  const char *name;
  GridSolverFunction *solve;
  GridSolverParams *params;
};

extern GridSolverFunction red_rec;
extern GridSolverFunction red_rec_deferred_solving;
extern GridSolverFunction red_rec_parallel;
extern GridSolverFunction red_rec_parallel_single_consumer;
extern GridSolverFunction red_rec_parallel_multiple_consumers;

enum ColumnPairPriorityQueueType {
  ARRAY_PRIORITY_QUEUE,
  HEAP_PRIORITY_QUEUE,
};

typedef struct {
  struct LinearSolver *linear_solver;
  enum ColumnPairPriorityQueueType pq_type;
} RedRecParams;

typedef struct {
  struct LinearSolver *linear_solver;
  enum ColumnPairPriorityQueueType pq_type;
  int thread_num;
} RedRecParallelParams;
