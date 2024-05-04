#include <string.h>

#include "../red_rec_utils/red_rec_utils.h"
#include "./red_rec_parallel_utils.h"

void compute_counts_parallel(const struct Grid *grid,
                             struct Counts *column_counts,
                             struct Counts *total_counts,
                             pthread_mutex_t *mutex,
                             struct Range column_range) {
  struct Counts private_total_counts = {0, 0};

  for (int i = column_range.start; i < column_range.exclusive_end; i++) {
    column_counts[i - column_range.start] =
        interval_get_counts(&(struct Interval){
            .array = grid_get_column(grid, i),
            .length = grid->height,
        });

    private_total_counts.source_num +=
        column_counts[i - column_range.start].source_num;
    private_total_counts.target_num +=
        column_counts[i - column_range.start].target_num;
  }

  pthread_mutex_lock(mutex);
  *total_counts = (struct Counts){
      .source_num = total_counts->source_num + private_total_counts.source_num,
      .target_num = total_counts->target_num + private_total_counts.target_num,
  };
  pthread_mutex_unlock(mutex);
}

void solve_donors_parallel(const struct RedRecThreadInputContext context,
                           const struct Counts *private_column_counts,
                           const struct LinearSolver *linear_solver,
                           pthread_mutex_t *mutex, struct Range column_range) {
  struct Reconfiguration *private_reconfiguration = reconfiguration_new(
      context.grid->width *
      (context.target_range.exclusive_end - context.target_range.start));

  for (int i = column_range.start; i < column_range.exclusive_end; i++) {
    if (counts_get_imbalance(private_column_counts[i - column_range.start]) >=
        0) {
      solve_donor(context.grid, private_reconfiguration, i, linear_solver);
    }
  }

  pthread_mutex_lock(mutex);
  int reconfiguration_initial_length = context.reconfiguration->move_count;
  context.reconfiguration->move_count += private_reconfiguration->move_count;
  pthread_mutex_unlock(mutex);

  memcpy(&context.reconfiguration->moves[reconfiguration_initial_length],
         private_reconfiguration->moves,
         private_reconfiguration->move_count *
             sizeof(private_reconfiguration->moves[0]));

  reconfiguration_free(private_reconfiguration);
}
