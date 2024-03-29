#include <string.h>

#include "../red_rec_utils/red_rec_utils.h"
#include "./red_rec_parallel_utils.h"

void compute_counts_and_solve_donors_parallel(
    struct RedRecThreadInputContext context, struct Counts *column_counts,
    struct Counts *total_counts, const struct LinearSolver *linear_solver,
    pthread_mutex_t *reconfiguration_mutex, struct Range column_range) {
  struct Counts private_counts = {0, 0};
  struct Reconfiguration *private_reconfiguration = reconfiguration_new(
      context.grid->width *
      (context.target_range.exclusive_end - context.target_range.start));

  for (int i = column_range.start; i < column_range.exclusive_end; i++) {
    column_counts[i] = interval_get_counts(&(struct Interval){
        .array = grid_get_column(context.grid, i),
        .length = context.grid->height,
    });

    private_counts.source_num += column_counts[i].source_num;
    private_counts.target_num += column_counts[i].target_num;

    if (counts_get_imbalance(column_counts[i]) >= 0) {
      solve_donor(context.grid, private_reconfiguration, i, linear_solver);
    }
  }

  pthread_mutex_lock(reconfiguration_mutex);
  int reconfiguration_initial_length = context.reconfiguration->move_count;
  context.reconfiguration->move_count += private_reconfiguration->move_count;

  *total_counts = (struct Counts){
      .source_num = total_counts->source_num + private_counts.source_num,
      .target_num = total_counts->target_num + private_counts.target_num,
  };
  pthread_mutex_unlock(reconfiguration_mutex);

  memcpy(&context.reconfiguration->moves[reconfiguration_initial_length],
         private_reconfiguration->moves,
         private_reconfiguration->move_count *
             sizeof(private_reconfiguration->moves[0]));

  reconfiguration_free(private_reconfiguration);
}
