#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "../../src/solvers/solvers.h"
#include "./main.h"
#include "./solvers/test_cases.h"
#include "./solvers/test_solvers.h"

const char output_dir_name[] = "performance_results";
const char output_file_format[] = "./%s/%u.csv";

static struct Interval *interval_generator(const int size,
                                           const int imbalance) {
  return interval_factory.generate_interval_by_imbalance(size, imbalance);
}

const static int sizes[] = {
    200,  400,  600,  800,  1000, 1200, 1400, 1600, 1800, 2000,
    2200, 2400, 2600, 2800, 3000, 3200, 3400, 3600, 3800, 4000,
    4200, 4400, 4600, 4800, 5000, 5200, 5400, 5600, 5800, 6000,
};
const static double imbalance_percentages[] = {
    0, 1, 2, 3, 5, 7, 10, 20, 50, 100,
};

const static struct PerformanceTestCasesConfig config = {
    .sizes_num = sizeof(sizes) / sizeof(sizes[0]),
    .interval_sizes = sizes,
    .imbalance_percentages = imbalance_percentages,
    .imbalance_percentages_num =
        sizeof(imbalance_percentages) / sizeof(imbalance_percentages[0]),
    .repetitions_per_test_case = 5,
    .interval_generator = interval_generator,
};

int main() {
  printf("RUNNING PERFORMANCE TESTS\n");
  unsigned int seed = (unsigned int)time(NULL);
  srand(seed);
  printf("Seed set to %u\n", seed);

  const struct PerformanceTestCases test_cases =
      generate_performance_tests(config);
  struct Performance *const result = test_solvers_performance(&test_cases);
  const int result_size = test_cases.intervals_num * solvers_num;

  mkdir(output_dir_name, S_IRWXU | S_IRWXG | S_IRWXO);
  char *const output_file_name =
      malloc(sizeof(output_dir_name) + sizeof(output_file_format) +
             (int)log10(seed) + 1);
  sprintf(output_file_name, output_file_format, output_dir_name, seed);

  FILE *output_file = fopen(output_file_name, "w");
  performance_write_to_csv(result, result_size, output_file);
  fclose(output_file);
  printf("Output written to %s\n", output_file_name);

  free(output_file_name);
  free(result);
  performance_test_cases_free(test_cases);
  return 0;
};
