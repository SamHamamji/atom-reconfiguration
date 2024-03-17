#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "../../lib/grid_solver/grid_solver.h"
#include "../../lib/linear_solver/linear_solver.h"
#include "../../lib/utils/max_min.h"
#include "../../lib/utils/seed.h"
#include "./performance.h"
#include "./test_grid_solvers.h"

const char output_dir_name[] = "performance_results/grid_solvers";
const char output_file_format[] = "./%s/%u.csv";

static inline struct Grid *grid_generator(struct GridSize size,
                                          double imbalance_percentage) {
  int target_region_height = size.height / 2;
  int imbalance = (int)(size.width * size.height * imbalance_percentage / 100);
  return grid_factory.generate_compact_target_region_by_imbalance(
      size.width, size.height, target_region_height, imbalance);
}

static const struct GridSize sizes[] = {
    {500, 500},   {1000, 500},  {1500, 500},  {2000, 500},  {2500, 500},
    {3000, 500},  {3500, 500},  {4000, 500},  {4500, 500},  {5000, 500},
    {500, 1000},  {1000, 1000}, {1500, 1000}, {2000, 1000}, {2500, 1000},
    {3000, 1000}, {3500, 1000}, {4000, 1000}, {4500, 1000}, {5000, 1000},
    {500, 1500},  {1000, 1500}, {1500, 1500}, {2000, 1500}, {2500, 1500},
    {3000, 1500}, {3500, 1500}, {4000, 1500}, {4500, 1500}, {5000, 1500},
    {500, 2000},  {1000, 2000}, {1500, 2000}, {2000, 2000}, {2500, 2000},
    {3000, 2000}, {3500, 2000}, {4000, 2000}, {4500, 2000}, {5000, 2000},
    {500, 2500},  {1000, 2500}, {1500, 2500}, {2000, 2500}, {2500, 2500},
    {3000, 2500}, {3500, 2500}, {4000, 2500}, {4500, 2500}, {5000, 2500},
    {500, 3000},  {1000, 3000}, {1500, 3000}, {2000, 3000}, {2500, 3000},
    {3000, 3000}, {3500, 3000}, {4000, 3000}, {4500, 3000}, {5000, 3000},
    {500, 3500},  {1000, 3500}, {1500, 3500}, {2000, 3500}, {2500, 3500},
    {3000, 3500}, {3500, 3500}, {4000, 3500}, {4500, 3500}, {5000, 3500},
    {500, 4000},  {1000, 4000}, {1500, 4000}, {2000, 4000}, {2500, 4000},
    {3000, 4000}, {3500, 4000}, {4000, 4000}, {4500, 4000}, {5000, 4000},
    {500, 4500},  {1000, 4500}, {1500, 4500}, {2000, 4500}, {2500, 4500},
    {3000, 4500}, {3500, 4500}, {4000, 4500}, {4500, 4500}, {5000, 4500},
    {500, 5000},  {1000, 5000}, {1500, 5000}, {2000, 5000}, {2500, 5000},
    {3000, 5000}, {3500, 5000}, {4000, 5000}, {4500, 5000}, {5000, 5000},
};

static const double imbalance_percentages[] = {
    0, 1, 2, 5, 25, 50,
};

static struct LinearSolver default_linear_solver = {
    .solve = linear_solve_aggarwal,
    .params = NULL,
    .name = "Aggarwal solver",
};

static const struct GridSolver *grid_solvers[] = {
    &(struct GridSolver){
        .solve = red_rec,
        .params =
            &(RedRecParams){
                .linear_solver = &default_linear_solver,
            },
        .name = "Red rec",
    },
    &(struct GridSolver){
        .solve = red_rec_deferred_solving,
        .params =
            &(RedRecParams){
                .linear_solver = &default_linear_solver,
            },
        .name = "Red rec deferred solving",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .thread_num = 1,
            },
        .name = "Red rec parallel (1 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .thread_num = 3,
            },
        .name = "Red rec parallel (3 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .thread_num = 8,
            },
        .name = "Red rec parallel (8 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel_single_consumer,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .thread_num = 1,
            },
        .name = "Red rec parallel single consumer (1 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel_single_consumer,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .thread_num = 3,
            },
        .name = "Red rec parallel single consumer (3 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel_single_consumer,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .thread_num = 8,
            },
        .name = "Red rec parallel single consumer (8 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel_multiple_consumers,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .thread_num = 1,
            },
        .name = "Red rec parallel multiple consumers (1 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel_multiple_consumers,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .thread_num = 3,
            },
        .name = "Red rec parallel multiple consumers (3 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel_multiple_consumers,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .thread_num = 8,
            },
        .name = "Red rec parallel multiple consumers (8 threads)",
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
    .repetition_num = 5,
};

static char *get_output_file_name(unsigned int seed) {
  char *output_file_name =
      malloc(sizeof(output_dir_name) + sizeof(output_file_format) +
             (int)log10(max(seed, 1)) + 1);
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
