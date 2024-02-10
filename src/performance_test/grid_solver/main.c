#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "../../lib/grid_solver/grid_solver.h"
#include "../../lib/linear_solver/linear_solver.h"
#include "../../lib/utils/seed.h"
#include "./performance.h"
#include "./test_grid_solvers.h"

const char output_dir_name[] = "performance_results";
const char output_file_format[] = "./%s/%u.csv";

static inline struct Grid *grid_generator(struct GridSize size,
                                          double imbalance_percentage) {
  int target_region_height = size.height / 2;
  int imbalance = (int)(size.width * size.height * imbalance_percentage / 100);
  return grid_factory.generate_compact_target_region_by_imbalance(
      size.width, size.height, target_region_height, imbalance);
}

static const struct GridSize sizes[] = {
    // {100, 100}, {150, 100}, {200, 100}, {100, 150}, {150, 150},
    // {200, 150}, {100, 200}, {150, 200}, {200, 200},
};

static const double imbalance_percentages[] = {
    0, 1, 5, 25, 50,
};

static const struct GridSolver *grid_solvers[] = {
    &(struct GridSolver){
        .solve = red_rec,
        .params =
            &(RedRecParams){
                .linear_solver =
                    &(struct LinearSolver){
                        .solve = linear_solve_aggarwal_parallel,
                        .params =
                            &(AggarwalParallelOnChainsParams){.thread_num = 1},
                        .name = "Aggarwal solver parallel (1 thread)",
                    },
            },
        .name = "Red rec (aggarwal serial)",
    },
};

static const struct PerformanceTestCasesConfig config = {
    .grid_sizes = sizes,
    .imbalance_percentages = imbalance_percentages,
    .grid_solvers = grid_solvers,
    .grid_generator = grid_generator,
    .grid_sizes_num = sizeof(sizes) / sizeof(sizes[0]),
    .imbalance_percentages_num =
        sizeof(imbalance_percentages) / sizeof(imbalance_percentages[0]),
    .grid_solvers_num = sizeof(grid_solvers) / sizeof(grid_solvers[0]),
    .repetition_num = 1,
};

static char *get_output_file_name(unsigned int seed) {
  char *output_file_name =
      malloc(sizeof(output_dir_name) + sizeof(output_file_format) +
             (int)log10(seed) + 1);
  sprintf(output_file_name, output_file_format, output_dir_name, seed);
  return output_file_name;
}

int main() {
  printf("RUNNING PERFORMANCE TESTS\n");
  unsigned int seed = (unsigned int)time(NULL);
  seed_set(seed);

  struct PerformanceArray *results = test_grid_solvers_performance(&config);

  char *output_file_name = get_output_file_name(seed);
  mkdir(output_dir_name, S_IRWXU | S_IRWXG | S_IRWXO);

  performance_write_to_csv(results, output_file_name);
  printf("Output written to %s\n", output_file_name);

  free(output_file_name);
  performance_array_free(results);
  return EXIT_SUCCESS;
}
