#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "../lib/linear_solver/linear_solver.h"
#include "../lib/utils/seed.h"
#include "linear_solver/performance.h"
#include "linear_solver/test_linear_solvers.h"

const char output_dir_name[] = "performance_results";
const char output_file_format[] = "./%s/%u.csv";

static inline struct Interval *interval_generator(const int length,
                                                  const int imbalance) {
  return interval_factory.generate_by_imbalance(length, imbalance);
}

static const int lengths[] = {
    10000,   20000,   40000,   80000,    160000,   320000,   640000,
    1280000, 2560000, 5120000, 10000000, 20000000, 40000000, 80000000,
};

static const double imbalance_percentages[] = {
    0, 1, 5, 25, 100,
    // 0, 1, 3, 9, 27, 100,
    // 0, 1, 4, 16, 64,
};

static const struct LinearSolver *config_linear_solvers[] = {
    &(struct LinearSolver){
        .solve = linear_solve_aggarwal,
        .params = NULL,
        .name = "Aggarwal solver",
    },
    &(struct LinearSolver){
        .solve = linear_solve_aggarwal_parallel,
        .params = &(AggarwalParallelOnChainsParams){.thread_num = 1},
        .name = "Aggarwal solver parallel (1 thread)",
    },
    &(struct LinearSolver){
        .solve = linear_solve_aggarwal_parallel,
        .params = &(AggarwalParallelOnChainsParams){.thread_num = 2},
        .name = "Aggarwal solver parallel (2 threads)",
    },
    &(struct LinearSolver){
        .solve = linear_solve_aggarwal_parallel,
        .params = &(AggarwalParallelOnChainsParams){.thread_num = 4},
        .name = "Aggarwal solver parallel (4 threads)",
    },
    &(struct LinearSolver){
        .solve = linear_solve_aggarwal_parallel,
        .params = &(AggarwalParallelOnChainsParams){.thread_num = 6},
        .name = "Aggarwal solver parallel (6 threads)",
    },
    &(struct LinearSolver){
        .solve = linear_solve_aggarwal_parallel,
        .params = &(AggarwalParallelOnChainsParams){.thread_num = 8},
        .name = "Aggarwal solver parallel (8 threads)",
    },
};

static const struct PerformanceTestCasesConfig config = {
    .interval_lengths = lengths,
    .imbalance_percentages = imbalance_percentages,
    .linear_solvers = config_linear_solvers,
    .interval_generator = interval_generator,
    .lengths_num = sizeof(lengths) / sizeof(lengths[0]),
    .imbalance_percentages_num =
        sizeof(imbalance_percentages) / sizeof(imbalance_percentages[0]),
    .linear_solvers_num =
        sizeof(config_linear_solvers) / sizeof(config_linear_solvers[0]),
    .repetition_num = 2,
};

char *get_output_file_name(unsigned int seed) {
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

  struct PerformanceArray *results = test_linear_solvers_performance(&config);

  char *output_file_name = get_output_file_name(seed);
  mkdir(output_dir_name, S_IRWXU | S_IRWXG | S_IRWXO);

  performance_write_to_csv(results, output_file_name);
  printf("Output written to %s\n", output_file_name);

  free(output_file_name);
  performance_array_free(results);
  return EXIT_SUCCESS;
}
