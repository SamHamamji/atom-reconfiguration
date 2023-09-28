#include "./performance.h"

const static char csv_header[] = "solver,size,imbalance,time_taken\n";

void performance_write_to_csv(const struct Performance *const performance,
                              const int performance_count, FILE *const file) {
  fprintf(file, csv_header);
  for (int i = 0; i < performance_count; i++) {
    fprintf(file, "%s,%d,%d,%f\n", performance[i].solver->name,
            performance[i].interval_size, performance[i].imbalance,
            performance[i].time_taken);
  }
}
