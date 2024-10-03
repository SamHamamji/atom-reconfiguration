#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "../../lib/grid_solver/grid_solver.h"
#include "../../lib/linear_solver/linear_solver.h"
#include "../../lib/utils/integer_length.h"
#include "../../lib/utils/seed.h"
#include "./performance.h"
#include "./test_grid_solvers.h"

const char results_dir[] = "performance_results";
const char grid_results_dir[] = "performance_results/grid_solvers";

const char output_file_format[] = "./%s/%u.csv";

static inline struct Grid *grid_generator(struct GridSize size,
                                          double imbalance_percentage) {
  int target_region_height = size.height / 2;
  int imbalance = (int)(size.width * size.height * imbalance_percentage / 100);
  return grid_factory.generate_compact_target_region_by_imbalance(
      size.width, size.height, target_region_height, imbalance);
}

static const struct GridSize sizes[] = {
    {400, 400},  {800, 400},  {1200, 400},  {1600, 400},  {2000, 400},
    {400, 800},  {800, 800},  {1200, 800},  {1600, 800},  {2000, 800},
    {400, 1200}, {800, 1200}, {1200, 1200}, {1600, 1200}, {2000, 1200},
    {400, 1600}, {800, 1600}, {1200, 1600}, {1600, 1600}, {2000, 1600},
    {400, 2000}, {800, 2000}, {1200, 2000}, {1600, 2000}, {2000, 2000},
};

static const double imbalance_percentages[] = {
    0, 0, 0, 0, 0, 0, 0.5, 0.5, 0.5, 0.5, 1, 1, 1, 2, 3, 5, 8,
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
                .pq_type = ARRAY_PRIORITY_QUEUE,
                .receiver_solving_order = ALTERNATED_SOLVING,
            },
        .name = "Red rec",
    },
    &(struct GridSolver){
        .solve = red_rec,
        .params =
            &(RedRecParams){
                .linear_solver = &default_linear_solver,
                .pq_type = HEAP_PRIORITY_QUEUE,
                .receiver_solving_order = ALTERNATED_SOLVING,
            },
        .name = "Red rec heap priority queue",
    },
    &(struct GridSolver){
        .solve = red_rec,
        .params =
            &(RedRecParams){
                .linear_solver = &default_linear_solver,
                .pq_type = ARRAY_PRIORITY_QUEUE,
                .receiver_solving_order = DEFERRED_SOLVING,
            },
        .name = "Red rec deferred solving",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .pq_type = ARRAY_PRIORITY_QUEUE,
                .thread_num = 1,
            },
        .name = "Red rec parallel (1 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .pq_type = ARRAY_PRIORITY_QUEUE,
                .thread_num = 2,
            },
        .name = "Red rec parallel (2 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .pq_type = ARRAY_PRIORITY_QUEUE,
                .thread_num = 4,
            },
        .name = "Red rec parallel (4 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .pq_type = ARRAY_PRIORITY_QUEUE,
                .thread_num = 8,
            },
        .name = "Red rec parallel (8 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .pq_type = HEAP_PRIORITY_QUEUE,
                .thread_num = 8,
            },
        .name = "Red rec parallel (8 threads) heap priority queue",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel_single_consumer,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .pq_type = ARRAY_PRIORITY_QUEUE,
                .thread_num = 1,
            },
        .name = "Red rec parallel single consumer (1 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel_single_consumer,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .pq_type = ARRAY_PRIORITY_QUEUE,
                .thread_num = 2,
            },
        .name = "Red rec parallel single consumer (2 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel_single_consumer,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .pq_type = ARRAY_PRIORITY_QUEUE,
                .thread_num = 4,
            },
        .name = "Red rec parallel single consumer (4 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel_single_consumer,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .pq_type = ARRAY_PRIORITY_QUEUE,
                .thread_num = 8,
            },
        .name = "Red rec parallel single consumer (8 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel_single_consumer,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .pq_type = HEAP_PRIORITY_QUEUE,
                .thread_num = 8,
            },
        .name =
            "Red rec parallel single consumer (8 threads) heap priority queue",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel_multiple_consumers,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .pq_type = ARRAY_PRIORITY_QUEUE,
                .thread_num = 1,
            },
        .name = "Red rec parallel multiple consumers (1 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel_multiple_consumers,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .pq_type = ARRAY_PRIORITY_QUEUE,
                .thread_num = 2,
            },
        .name = "Red rec parallel multiple consumers (2 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel_multiple_consumers,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .pq_type = ARRAY_PRIORITY_QUEUE,
                .thread_num = 4,
            },
        .name = "Red rec parallel multiple consumers (4 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel_multiple_consumers,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .pq_type = ARRAY_PRIORITY_QUEUE,
                .thread_num = 8,
            },
        .name = "Red rec parallel multiple consumers (8 threads)",
    },
    &(struct GridSolver){
        .solve = red_rec_parallel_multiple_consumers,
        .params =
            &(RedRecParallelParams){
                .linear_solver = &default_linear_solver,
                .pq_type = HEAP_PRIORITY_QUEUE,
                .thread_num = 8,
            },
        .name = "Red rec parallel multiple consumers (8 threads) heap priority "
                "queue",
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
    .repetition_num = 2,
};

static char *get_output_file_name(unsigned int seed) {
  char *output_file_name =
      malloc(sizeof(grid_results_dir) + sizeof(output_file_format) +
             integer_length(seed));
  sprintf(output_file_name, output_file_format, grid_results_dir, seed);
  return output_file_name;
}

int main() {
  printf("RUNNING PERFORMANCE TESTS\n");
  unsigned int seed = (unsigned int)time(NULL);
  seed_set(seed);

  struct PerformanceArray *results = test_grid_solvers_performance(&config);

  char *output_file_name = get_output_file_name(seed);
  mkdir(results_dir, S_IRWXU | S_IRWXG | S_IRWXO);
  mkdir(grid_results_dir, S_IRWXU | S_IRWXG | S_IRWXO);

  performance_write_to_csv(results, output_file_name);
  printf("Output written to %s\n", output_file_name);

  free(output_file_name);
  performance_array_free(results);
  return EXIT_SUCCESS;
}
