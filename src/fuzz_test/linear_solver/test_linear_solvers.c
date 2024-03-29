#include <assert.h>
#include <stdio.h>

#include "../../lib/interval/interval.h"
#include "../../lib/interval/mapping.h"
#include "../../lib/linear_solver/linear_solver.h"
#include "../../lib/utils/colors.h"
#include "../../lib/utils/timer.h"
#include "./test_linear_solvers.h"

static void print_failed_test_case(
    const struct Interval *interval, const struct Mapping *initial_mapping,
    const struct LinearSolver *initial_linear_solver,
    const struct Mapping *mapping, const struct LinearSolver *linear_solver) {
  printf("Mappings do not match!\n");
  printf("Interval:\n");
  interval_print(interval);
  printf("Mapping from %s:\n", initial_linear_solver->name);
  mapping_print(initial_mapping);
  printf("Mapping from %s:\n", linear_solver->name);
  mapping_print(mapping);
}

bool fuzz_test_linear_solvers(const struct LinearSolversFuzzTestConfig config) {
  printf(BOLD "Testing linear solvers...\n" RESET);

  assert(config.linear_solvers_num >= 2);

  struct Timer timer;
  timer_start(&timer);

  bool success = true;
  int test_case_num = 0;
  while (success && timer_get_seconds(&timer) < config.time_limit_in_seconds) {
    int length = get_random_int_in_range(config.length_range);
    struct Interval *interval = config.interval_generator(
        length, get_random_int_in_range((struct Range){
                    .start = -length,
                    .exclusive_end = length,
                }));

    const struct LinearSolver *initial_linear_solver = config.linear_solvers[0];

    struct Mapping *initial_mapping =
        initial_linear_solver->solve(interval, initial_linear_solver->params);

    for (int i = 1; success && i < config.linear_solvers_num; i++) {
      const struct LinearSolver *linear_solver = config.linear_solvers[i];

      struct Mapping *mapping =
          linear_solver->solve(interval, linear_solver->params);

      if (!mapping_equals(initial_mapping, mapping)) {
        success = false;
        print_failed_test_case(interval, initial_mapping, initial_linear_solver,
                               mapping, linear_solver);
      }
      mapping_free(mapping);
    }

    mapping_free(initial_mapping);
    interval_free(interval);

    test_case_num++;
    timer_stop(&timer);
  }

  if (success) {
    printf("%d linear solver tests passed in %.2f seconds!\n", test_case_num,
           config.time_limit_in_seconds);
  } else {
    printf("🔴 Failed linear solver test case %d.\n", test_case_num);
  }

  return success;
}
