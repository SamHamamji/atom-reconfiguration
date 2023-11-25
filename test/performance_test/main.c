#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "../../src/solvers/solvers.h"
#include "./main.h"
#include "./solvers/test_cases.h"
#include "./solvers/test_solvers.h"
#include "solvers/performance.h"

const char output_dir_name[] = "performance_results";
const char output_file_format[] = "./%s/%u.csv";

static struct Interval *interval_generator(const int size,
                                           const int imbalance) {
  return interval_factory.generate_interval_by_imbalance(size, imbalance);
}

const static int sizes[] = {
    50,  100, 150, 200, 250, 300, 350, 400, 450, 500,
    550, 600, 650, 700, 750, 800, 850, 900, 950, 1000,
};
const static double imbalance_percentages[] = {
    0, 1, 2, 3, 5, 7, 10, 20, 50, 100,
};

const static struct Solver *config_solvers[] = {
    &iterative_solver,
    &parallel_solver,
    &karp_li_solver,
    &aggarwal_solver,
};

const static struct PerformanceTestCasesConfig config = {
    .interval_sizes = sizes,
    .imbalance_percentages = imbalance_percentages,
    .solvers = config_solvers,
    .interval_generator = interval_generator,
    .sizes_num = sizeof(sizes) / sizeof(sizes[0]),
    .imbalance_percentages_num =
        sizeof(imbalance_percentages) / sizeof(imbalance_percentages[0]),
    .solvers_num = sizeof(config_solvers) / sizeof(config_solvers[0]),
    .repetitions_per_test_case = 5,
};

char *get_output_file_name(unsigned int seed) {
  char *const output_file_name =
      malloc(sizeof(output_dir_name) + sizeof(output_file_format) +
             (int)log10(seed) + 1);
  sprintf(output_file_name, output_file_format, output_dir_name, seed);
  return output_file_name;
}

int main() {
  printf("RUNNING PERFORMANCE TESTS\n");
  unsigned int seed = (unsigned int)time(NULL);
  srand(seed);
  printf("Seed set to %u\n", seed);

  struct PerformanceArray *const results = test_solvers_performance(&config);

  char *const output_file_name = get_output_file_name(seed);
  mkdir(output_dir_name, S_IRWXU | S_IRWXG | S_IRWXO);

  performance_write_to_csv(results, output_file_name);
  printf("Output written to %s\n", output_file_name);

  free(output_file_name);
  performance_array_free(results);
  return 0;
};
