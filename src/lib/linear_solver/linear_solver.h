#pragma once

#include "../interval/interval.h"
#include "../interval/mapping.h"

typedef void LinearSolverParams;
typedef struct Mapping *LinearSolverFunction(const struct Interval *interval,
                                             const LinearSolverParams *params);

struct LinearSolver {
  const char *name;
  LinearSolverFunction *solve;
  LinearSolverParams *params;
};

extern LinearSolverFunction linear_solve_iterative;
extern LinearSolverFunction linear_solve_karp_li;
extern LinearSolverFunction linear_solve_karp_li_parallel;
extern LinearSolverFunction linear_solve_aggarwal;
extern LinearSolverFunction linear_solve_compact_target_region_aggarwal;
extern LinearSolverFunction linear_solve_aggarwal_parallel;
extern LinearSolverFunction linear_solve_aggarwal_parallel_on_chains;
extern LinearSolverFunction linear_solve_aggarwal_parallel_on_neutral;

struct NoParams {};
struct ParallelParams {
  int thread_num;
};
typedef struct NoParams IterativeParams;
typedef struct NoParams KarpLiParams;
typedef struct ParallelParams KarpLiParallelParams;
typedef struct NoParams AggarwalParams;
typedef struct NoParams AggarwalCompactTargetRegionParams;
typedef struct ParallelParams AggarwalParallelParams;
typedef struct ParallelParams AggarwalParallelOnChainsParams;
typedef struct ParallelParams AggarwalParallelOnNeutralParams;