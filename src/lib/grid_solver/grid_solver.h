#pragma once

#include "../grid/reconfiguration.h"

typedef void GridSolverParams;
typedef struct Reconfiguration *
GridSolverFunction(const struct Grid *grid, const GridSolverParams *params);

struct GridSolver {
  const char *name;
  GridSolverFunction *solve;
  GridSolverParams *params;
};

extern GridSolverFunction red_rec;

struct RedRecParams {
  struct LinearSolver *linear_solver;
};
typedef struct RedRecParams RedRecParams;
