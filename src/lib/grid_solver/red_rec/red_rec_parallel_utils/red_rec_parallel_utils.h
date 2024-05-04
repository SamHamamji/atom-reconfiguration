#pragma once

#include <pthread.h>

#include "../../grid_solver.h"

struct RedRecThreadInputContext {
  struct Grid *grid;
  struct Reconfiguration *reconfiguration;
  struct Range target_range;
};

void compute_counts_parallel(const struct Grid *grid,
                             struct Counts *column_counts,
                             struct Counts *total_counts,
                             pthread_mutex_t *mutex, struct Range column_range);

void solve_donors_parallel(const struct RedRecThreadInputContext context,
                           const struct Counts *private_column_counts,
                           const struct LinearSolver *linear_solver,
                           pthread_mutex_t *mutex, struct Range column_range);
