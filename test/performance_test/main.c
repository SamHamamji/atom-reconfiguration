#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../src/solvers/solvers.h"
#include "./main.h"
#include "./solvers/test_cases.h"
#include "./solvers/test_solvers_performance.h"

int main() {
  printf("RUNNING PERFORMANCE TESTS\n");
  unsigned int seed = (unsigned int)time(NULL);
  srand(seed);
  printf("Random seed set to %u\n", seed);

  const int sizes[] = {16, 64, 256, 1024, 4096, 8192};
  const struct PerformanceTestCasesConfig config = {
      .sizes_num = sizeof(sizes) / sizeof(sizes[0]),
      .interval_sizes = sizes,
      .tests_per_size = 1,
  };

  struct PerformanceTestCases test_cases = generate_performance_tests(config);
  struct Performance *const result = test_solvers_performance(&test_cases);

  for (int i = 0; i < test_cases.intervals_num * solvers_num; i++) {
    printf("%s took %f seconds to execute %d array with %d imbalance\n",
           result[i].solver->name, result[i].time_taken,
           result[i].interval_size, result[i].imbalance);
  }

  free(result);
  performance_test_cases_free(test_cases);
  return 0;
};
