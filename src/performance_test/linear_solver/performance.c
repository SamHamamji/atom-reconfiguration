#include <stdio.h>
#include <stdlib.h>

#include "./performance.h"

static const char csv_header[] =
    "linear_solver,length,imbalance_percentage,time_taken\n";

void performance_write_to_csv(const struct PerformanceArray *performance,
                              const char *file_name) {
  FILE *file = fopen(file_name, "w");
  fprintf(file, csv_header);
  for (int i = 0; i < performance->length; i++) {
    fprintf(file, "%s,%d,%f,%f\n",
            performance->performances[i].linear_solver->name,
            performance->performances[i].interval_length,
            performance->performances[i].imbalance_percentage,
            performance->performances[i].time_taken);
  }
  fclose(file);
}

void performance_array_free(struct PerformanceArray *performance_array) {
  free(performance_array->performances);
  free(performance_array);
}
