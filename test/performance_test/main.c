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

const static int sizes[] = {
    16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384,
};
const static struct PerformanceTestCasesConfig config = {
    .sizes_num = sizeof(sizes) / sizeof(sizes[0]),
    .interval_sizes = sizes,
    .tests_per_size = 100,
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
