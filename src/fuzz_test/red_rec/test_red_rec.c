#include <stdio.h>
#include <stdlib.h>

#include "../../lib/grid/reconfiguration.h"
#include "../../lib/red_rec/red_rec.h"
#include "../../lib/utils/colors.h"
#include "../../lib/utils/timer.h"
#include "./test_red_rec.h"

static int get_random_int(struct Range range) {
  return range.start + rand() % (range.exclusive_end - range.start);
}

static void print_failed_test_case(const struct Grid *initial_grid,
                                   const struct Grid *grid, int test_case_num) {
  printf("Grid is not solved! Failed test case %d.\n", test_case_num);
  printf("Initial grid:\n");
  grid_print(initial_grid);
  printf("Grid:\n");
  grid_print(grid);
}

bool fuzz_test_red_rec(const struct RedRecFuzzTestConfig config) {
  printf(BOLD "Testing red-rec...\n" RESET);

  struct Timer timer;
  timer_start(&timer);

  int test_case_num = 0;
  bool success = true;
  while (success && timer_get_seconds(&timer) < config.time_limit_in_seconds) {
    test_case_num++;

    struct Grid *grid =
        config.grid_generator(get_random_int(config.width_range),
                              get_random_int(config.height_range));
    struct Reconfiguration *reconfiguration = red_rec(grid);

    struct Grid *initial_grid = grid_get_copy(grid);
    grid_apply_reconfiguration(grid, reconfiguration);

    if (reconfiguration != NULL && !grid_is_solved(grid)) {
      success = false;
      print_failed_test_case(initial_grid, grid, test_case_num);
    }

    reconfiguration_free(reconfiguration);
    grid_free(initial_grid);
    grid_free(grid);

    timer_stop(&timer);
  }

  if (success) {
    printf("%d red-rec tests passed in %.2f seconds!\n", test_case_num,
           config.time_limit_in_seconds);
  } else {
    printf("🔴 Failed red-rec test case %d.\n", test_case_num);
  }

  return success;
}
