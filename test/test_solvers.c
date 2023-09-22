#include "../unity/unity.h"
#include <stdio.h>
#include <string.h>

#include "../src/mapping/mapping.h"
#include "./test_solvers.h"

#define NEW_SOLVER_TEST_CASE(interval, mapping)                                \
  ((struct SolverTestCase){                                                    \
      .input = {.array = interval,                                             \
                .size = sizeof(interval) / sizeof(interval[0])},               \
      .expected_output = {.pairs = mapping,                                    \
                          .pair_count = sizeof(mapping) / sizeof(mapping[0])}, \
  })

#define DEFINE_TEST_SOLVER_FUNCTION(solver, name)                              \
  void name(void) {                                                            \
    for (int i = 0; i < solver_test_cases_num; i++) {                          \
      struct Mapping *mapping = solver(&solver_test_cases[i].input);           \
      int mappings_are_equal =                                                 \
          mapping_equals(mapping, &solver_test_cases[i].expected_output);      \
      if (mappings_are_equal) {                                                \
        printf("Passed subtest %d\n", i + 1);                                  \
      } else {                                                                 \
        printf("Failed subtest %d\n", i + 1);                                  \
        printf("Expected:\n");                                                 \
        mapping_print(&solver_test_cases[i].expected_output);                  \
        printf("Got:\n");                                                      \
        mapping_print(mapping);                                                \
      }                                                                        \
      TEST_ASSERT(mappings_are_equal);                                         \
    }                                                                          \
  }

// Test 1
static Point interval_1[] = {EMPTY,  EMPTY,  SOURCE, TARGET, TARGET, SOURCE,
                             SOURCE, SOURCE, EMPTY,  TARGET, TARGET, EMPTY};
static struct Pair mapping_1[] = {{2, 3}, {5, 4}, {6, 9}, {7, 10}};

// Test 2
static Point interval_2[] = {EMPTY,  EMPTY,  SOURCE, SOURCE, TARGET, TARGET,
                             TARGET, SOURCE, TARGET, SOURCE, SOURCE, EMPTY,
                             TARGET, SOURCE, SOURCE, SOURCE, SOURCE, EMPTY,
                             TARGET, TARGET, EMPTY};
static struct Pair mapping_2[] = {
    {13, 12}, {3, 5}, {7, 6}, {16, 19}, {9, 8}, {2, 4}, {15, 18},
};

// Test 3
static Point interval_3[] = {EMPTY,  TARGET, TARGET, EMPTY,  EMPTY,  SOURCE,
                             SOURCE, SOURCE, SOURCE, TARGET, EMPTY,  SOURCE,
                             SOURCE, TARGET, SOURCE, TARGET, TARGET, TARGET,
                             SOURCE, SOURCE, EMPTY};
static struct Pair mapping_3[] = {
    {5, 1}, {6, 2}, {8, 9}, {12, 13}, {14, 15}, {18, 16}, {19, 17},
};

// Test 4
static Point interval_4[] = {EMPTY};
static struct Pair mapping_4[] = {};

static const struct SolverTestCase solver_test_cases[] = {
    NEW_SOLVER_TEST_CASE(interval_1, mapping_1),
    NEW_SOLVER_TEST_CASE(interval_2, mapping_2),
    NEW_SOLVER_TEST_CASE(interval_3, mapping_3),
    NEW_SOLVER_TEST_CASE(interval_4, mapping_4),
};

static const int solver_test_cases_num =
    sizeof(solver_test_cases) / sizeof(solver_test_cases[0]);

DEFINE_TEST_SOLVER_FUNCTION(iterative_solver, test_iterative_solver)
DEFINE_TEST_SOLVER_FUNCTION(karp_li_solver, test_karp_li_solver);
