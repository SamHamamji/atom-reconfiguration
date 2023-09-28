#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "../../src/solvers/solvers.h"
#include "./main.h"
#include "./solvers/test_cases.h"
#include "./solvers/test_solvers.h"

const static int sizes[] = {16, 64, 256, 1024, 2048, 4096, 8192, 16384};
const static struct PerformanceTestCasesConfig config = {
    .sizes_num = sizeof(sizes) / sizeof(sizes[0]),
    .interval_sizes = sizes,
    .tests_per_size = 10,
};

int main() {
  printf("RUNNING PERFORMANCE TESTS\n");
  unsigned int seed = (unsigned int)time(NULL);
  srand(seed);
  printf("Random seed set to %u\n", seed);

  struct PerformanceTestCases test_cases = generate_performance_tests(config);
  struct Performance *const result = test_solvers_performance(&test_cases);
  const int result_size = test_cases.intervals_num * solvers_num;

  for (int i = 0; i < result_size; i++) {
    printf("%s took %f seconds to execute %d array with %d imbalance\n",
           result[i].solver->name, result[i].time_taken,
           result[i].interval_size, result[i].imbalance);
  }

  char output_dir_name[] = "performance_results";
  mkdir(output_dir_name, S_IRWXU | S_IRWXG | S_IRWXO);

  char file_format[] = "./%s/%u.csv";
  char *output_file_name =
      malloc(sizeof(output_dir_name) + sizeof(file_format) + log10(seed) + 1);
  sprintf(output_file_name, file_format, output_dir_name, seed);

  FILE *output_file = fopen(output_file_name, "w");
  printf("%d\n", output_file == NULL);
  performance_write_to_csv(result, result_size, output_file);

  fclose(output_file);
  free(output_file_name);
  free(result);
  performance_test_cases_free(test_cases);
  return 0;
};
