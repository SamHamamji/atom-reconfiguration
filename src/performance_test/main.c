#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "../lib/solver/solver.h"
#include "./solvers/test_cases.h"
#include "./solvers/test_solvers.h"
#include "solvers/performance.h"

const char output_dir_name[] = "performance_results";
const char output_file_format[] = "./%s/%u.csv";

static struct Interval *interval_generator(const int size,
                                           const int imbalance) {
  return interval_factory.generate_interval_by_imbalance(size, imbalance);
}

static const int sizes[] = {
    // 25,  50,  75,  100, 125, 150, 175, 200, 225, 250, 275, 300,  325, 350,
    // 375, 400, 425, 450, 475, 500, 525, 550, 575, 600, 625, 650,  675, 700,
    // 725, 750, 775, 800, 825, 850, 875, 900, 925, 950, 975, 1000,

    // 100,  200,  300,  400,  500,  600,  700,  800,  900,  1000,
    // 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000,
    // 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900, 3000,
    // 3100, 3200, 3300, 3400, 3500, 3600, 3700, 3800, 3900, 4000,
    // 4100, 4200, 4300, 4400, 4500, 4600, 4700, 4800, 4900, 5000,

    10000,  20000,  30000,  40000,  50000,  60000,  70000,  80000,  90000,
    100000, 110000, 120000, 130000, 140000, 150000, 160000, 170000, 180000,
    190000, 200000, 210000, 220000, 230000, 240000, 250000, 260000, 270000,
    // 280000, 290000, 300000, 310000, 320000, 330000, 340000, 350000, 360000,
    // 370000, 380000, 390000, 400000, 410000, 420000, 430000, 440000, 450000,
    // 460000, 470000, 480000, 490000, 500000,
};
static const double imbalance_percentages[] = {
    0, 1, 3, 9, 27, 100,
    // 0, 1, 4, 16, 64,
};

static const struct Solver *config_solvers[] = {
    // &iterative_solver,
    // &karp_li_solver,
    &aggarwal_solver,
    &aggarwal_parallel_solver_on_chains,
    &aggarwal_parallel_solver,
};

static const struct PerformanceTestCasesConfig config = {
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
  char *output_file_name =
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

  struct PerformanceArray *results = test_solvers_performance(&config);

  char *output_file_name = get_output_file_name(seed);
  mkdir(output_dir_name, S_IRWXU | S_IRWXG | S_IRWXO);

  performance_write_to_csv(results, output_file_name);
  printf("Output written to %s\n", output_file_name);

  free(output_file_name);
  performance_array_free(results);
  return 0;
}
