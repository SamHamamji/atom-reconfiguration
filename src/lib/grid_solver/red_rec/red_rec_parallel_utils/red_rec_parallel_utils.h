#pragma once

#include <pthread.h>

#include "../../grid_solver.h"

struct RedRecThreadInputContext {
  struct Grid *grid;
  struct Reconfiguration *reconfiguration;
  struct Range target_range;
};

void compute_counts_and_solve_donors_parallel(
    struct RedRecThreadInputContext context, struct Counts *column_counts,
    struct Counts *total_counts, const struct LinearSolver *linear_solver,
    pthread_mutex_t *reconfiguration_mutex, struct Range column_range);
