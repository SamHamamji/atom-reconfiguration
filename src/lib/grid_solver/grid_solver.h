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

typedef struct {
  struct LinearSolver *linear_solver;
} RedRecParams;

typedef struct {
  struct LinearSolver *linear_solver;
  int thread_num;
} RedRecParallelParams;
