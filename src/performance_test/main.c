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
    // 25,  50,  75,  100, 125, 150, 175, 200, 225, 250, 275, 300,  325, 350,
    // 375, 400, 425, 450, 475, 500, 525, 550, 575, 600, 625, 650,  675, 700,
    // 725, 750, 775, 800, 825, 850, 875, 900, 925, 950, 975, 1000,

    100,  200,  300,  400,  500,  600,  700,  800,  900,  1000,
    1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000,
    2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900, 3000,
    3100, 3200, 3300, 3400, 3500, 3600, 3700, 3800, 3900, 4000,
    4100, 4200, 4300, 4400, 4500, 4600, 4700, 4800, 4900, 5000,

    // 2000000, 4000000, 6000000, 8000000, 10000000,
};

static const double imbalance_percentages[] = {
    1, 5, 25, 100,
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
  return 0;
}
