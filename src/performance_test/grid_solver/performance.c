#include <stdio.h>
#include <stdlib.h>

#include "./performance.h"

static const char csv_header[] =
    "grid_solver,width,height,imbalance_percentage,time_taken\n";

struct PerformanceArray *performance_array_new(int length) {
  struct PerformanceArray *performance_array =
      malloc(sizeof(struct PerformanceArray));
  performance_array->performances = malloc(length * sizeof(struct Performance));
  performance_array->length = length;
  return performance_array;
}

void performance_array_free(struct PerformanceArray *performance_array) {
  free(performance_array->performances);
  free(performance_array);
}

void performance_write_to_csv(const struct PerformanceArray *performance,
                              const char *file_name) {
  FILE *file = fopen(file_name, "w");
  fprintf(file, csv_header);
  for (int i = 0; i < performance->length; i++) {
    fprintf(file, "%s,%d,%d,%f,%f\n",
            performance->performances[i].grid_solver->name,
            performance->performances[i].grid_size.width,
            performance->performances[i].grid_size.height,
            performance->performances[i].imbalance_percentage,
            performance->performances[i].time_taken);
  }
  fclose(file);
}
