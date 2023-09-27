#include "test_cases.h"

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
static Point interval_4[] = {SOURCE, TARGET, SOURCE, TARGET, SOURCE,
                             TARGET, SOURCE, TARGET, SOURCE, TARGET,
                             SOURCE, TARGET, SOURCE, TARGET};
static struct Pair mapping_4[] = {
    {0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9}, {10, 11}, {12, 13},
};

// Test 5
static Point interval_5[] = {TARGET, SOURCE, SOURCE, SOURCE, TARGET, SOURCE,
                             TARGET, TARGET, TARGET, SOURCE, SOURCE};
static struct Pair mapping_5[] = {
    {1, 0}, {3, 4}, {5, 6}, {9, 7}, {10, 8},
};

// Test 6
static Point interval_6[] = {TARGET, TARGET, SOURCE, SOURCE,
                             SOURCE, SOURCE, SOURCE};
static struct Pair mapping_6[] = {{2, 0}, {3, 1}};

// Test 7
static Point interval_7[] = {TARGET, TARGET, EMPTY, EMPTY,
                             EMPTY,  SOURCE, SOURCE};
static struct Pair mapping_7[] = {{5, 0}, {6, 1}};

// Test 8
static Point interval_8[] = {SOURCE, SOURCE, TARGET, TARGET};
static struct Pair mapping_8[] = {{0, 2}, {1, 3}};

// Test 9
static Point interval_9[] = {SOURCE, SOURCE};
static struct Pair mapping_9[] = {};

// Test 10
static Point interval_10[] = {EMPTY};
static struct Pair mapping_10[] = {};

// Test 11
static Point interval_11[] = {};
static struct Pair mapping_11[] = {};

// Test 12
static Point interval_12[] = {TARGET};
static struct Pair mapping_12[] = {};

// Test 13
static Point interval_13[] = {TARGET, EMPTY,  SOURCE, EMPTY, TARGET, SOURCE,
                              TARGET, TARGET, EMPTY,  EMPTY, SOURCE};
static struct Pair mapping_13[] = {};

// Test 14
static Point interval_14[] = {TARGET, EMPTY, TARGET};
static struct Pair mapping_14[] = {};

#define NEW_SOLVER_TEST_CASE(interval, mapping)                                \
  ((struct SolverTestCase){                                                    \
      .input = {.array = interval,                                             \
                .size = sizeof(interval) / sizeof(interval[0])},               \
      .expected_output = {.pairs = mapping,                                    \
                          .pair_count = sizeof(mapping) / sizeof(mapping[0])}, \
  })

const struct SolverTestCase solver_test_cases[] = {
    NEW_SOLVER_TEST_CASE(interval_1, mapping_1),
    NEW_SOLVER_TEST_CASE(interval_2, mapping_2),
    NEW_SOLVER_TEST_CASE(interval_3, mapping_3),
    NEW_SOLVER_TEST_CASE(interval_4, mapping_4),
    NEW_SOLVER_TEST_CASE(interval_5, mapping_5),
    NEW_SOLVER_TEST_CASE(interval_6, mapping_6),
    NEW_SOLVER_TEST_CASE(interval_7, mapping_7),
    NEW_SOLVER_TEST_CASE(interval_8, mapping_8),
    NEW_SOLVER_TEST_CASE(interval_9, mapping_9),
    NEW_SOLVER_TEST_CASE(interval_10, mapping_10),
    NEW_SOLVER_TEST_CASE(interval_11, mapping_11),
    NEW_SOLVER_TEST_CASE(interval_12, mapping_12),
    NEW_SOLVER_TEST_CASE(interval_13, mapping_13),
    NEW_SOLVER_TEST_CASE(interval_14, mapping_14),
};

const int solver_test_cases_num =
    sizeof(solver_test_cases) / sizeof(solver_test_cases[0]);
