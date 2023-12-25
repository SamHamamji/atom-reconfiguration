#pragma once

#include "../interval/interval.h"
#include "../mapping/mapping.h"

typedef void SolverParams;
typedef struct Mapping *SolverFunction(const struct Interval *interval,
                                       const SolverParams *params);

struct Solver {
  const char *name;
  SolverFunction *solve;
  SolverParams *params;
};

extern SolverFunction iterative_solver_function;
extern SolverFunction karp_li_solver_function;
extern SolverFunction karp_li_parallel_solver_function;
extern SolverFunction aggarwal_solver_function;
extern SolverFunction aggarwal_parallel_solver_function;
extern SolverFunction aggarwal_parallel_on_chains_solver_function;
extern SolverFunction aggarwal_parallel_on_neutral_solver_function;

struct NoParams {};
struct ParallelSolverParams {
  int thread_num;
};
typedef struct NoParams IterativeParams;
typedef struct NoParams KarpLiParams;
typedef struct ParallelSolverParams KarpLiParallelParams;
typedef struct NoParams AggarwalParams;
typedef struct ParallelSolverParams AggarwalParallelParams;
typedef struct ParallelSolverParams AggarwalParallelOnChainsParams;
typedef struct ParallelSolverParams AggarwalParallelOnNeutralParams;
