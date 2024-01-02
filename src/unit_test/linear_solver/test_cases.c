#include "test_cases.h"

#define INTERVAL(points_array)                                                 \
  {                                                                            \
    .array = points_array,                                                     \
    .length = sizeof(points_array) / sizeof(points_array[0]),                  \
  }

#define MAPPING(pairs_array)                                                   \
  {                                                                            \
    .pairs = pairs_array,                                                      \
    .pair_count = sizeof(pairs_array) / sizeof(pairs_array[0]),                \
  }

#define NEW_LINEAR_SOLVER_TEST_CASE(interval, mapping)                         \
  { .input = INTERVAL(interval), .expected_output = MAPPING(mapping), }

// Test 1
static struct Point interval_1[] = {
    EMPTY,  EMPTY,  SOURCE, TARGET, TARGET, SOURCE,
    SOURCE, SOURCE, EMPTY,  TARGET, TARGET, EMPTY,
};
static struct Pair mapping_1[] = {{2, 3}, {5, 4}, {6, 9}, {7, 10}};

// Test 2
static struct Point interval_2[] = {
    EMPTY,  EMPTY,  SOURCE, SOURCE, TARGET, TARGET, TARGET,
    SOURCE, TARGET, SOURCE, SOURCE, EMPTY,  TARGET, SOURCE,
    SOURCE, SOURCE, SOURCE, EMPTY,  TARGET, TARGET, EMPTY,
};
static struct Pair mapping_2[] = {
    {13, 12}, {3, 5}, {7, 6}, {16, 19}, {9, 8}, {2, 4}, {15, 18},
};

// Test 3
static struct Point interval_3[] = {
    EMPTY,  TARGET, TARGET, EMPTY,  EMPTY,  SOURCE, SOURCE,
    SOURCE, SOURCE, TARGET, EMPTY,  SOURCE, SOURCE, TARGET,
    SOURCE, TARGET, TARGET, TARGET, SOURCE, SOURCE, EMPTY,
};
static struct Pair mapping_3[] = {
    {5, 1}, {6, 2}, {8, 9}, {12, 13}, {14, 15}, {18, 16}, {19, 17},
};

// Test 4
static struct Point interval_4[] = {
    SOURCE, TARGET, SOURCE, TARGET, SOURCE, TARGET, SOURCE,
    TARGET, SOURCE, TARGET, SOURCE, TARGET, SOURCE, TARGET,
};
static struct Pair mapping_4[] = {
    {0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9}, {10, 11}, {12, 13},
};

// Test 5
static struct Point interval_5[] = {
    TARGET, SOURCE, SOURCE, SOURCE, TARGET, SOURCE,
    TARGET, TARGET, TARGET, SOURCE, SOURCE,
};
static struct Pair mapping_5[] = {
    {1, 0}, {3, 4}, {5, 6}, {9, 7}, {10, 8},
};

// Test 6
static struct Point interval_6[] = {
    TARGET, TARGET, SOURCE, SOURCE, SOURCE, SOURCE, SOURCE,
};
static struct Pair mapping_6[] = {{2, 0}, {3, 1}};

// Test 7
static struct Point interval_7[] = {
    TARGET, TARGET, EMPTY, EMPTY, EMPTY, SOURCE, SOURCE,
};
static struct Pair mapping_7[] = {{5, 0}, {6, 1}};

// Test 8
static struct Point interval_8[] = {SOURCE, SOURCE, TARGET, TARGET};
static struct Pair mapping_8[] = {{0, 2}, {1, 3}};

// Test 9
static struct Point interval_9[] = {SOURCE, SOURCE};
static struct Pair mapping_9[] = {};

// Test 10
static struct Point interval_10[] = {EMPTY};
static struct Pair mapping_10[] = {};

// Test 11
static struct Point interval_11[] = {};
static struct Pair mapping_11[] = {};

// Test 12
static struct Point interval_12[] = {TARGET};
static struct Pair mapping_12[] = {};

// Test 13
static struct Point interval_13[] = {
    TARGET, EMPTY,  SOURCE, EMPTY, TARGET, SOURCE,
    TARGET, TARGET, EMPTY,  EMPTY, SOURCE,
};
static struct Pair mapping_13[] = {};

// Test 14
static struct Point interval_14[] = {TARGET, EMPTY, TARGET};
static struct Pair mapping_14[] = {};

// Test 15
static struct Point interval_15[] = {
    TARGET, SOURCE, SOURCE, TARGET, SOURCE, SOURCE, SOURCE, SOURCE, SOURCE,
    TARGET, SOURCE, SOURCE, SOURCE, EMPTY,  TARGET, EMPTY,  TARGET, TARGET,
    TARGET, EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  SOURCE, SOURCE, TARGET,
    TARGET, SOURCE, TARGET, EMPTY,  TARGET, EMPTY,
};
static struct Pair mapping_15[] = {
    {1, 0},   {2, 3},   {28, 31}, {25, 29}, {24, 27}, {12, 26},
    {11, 18}, {10, 17}, {8, 16},  {7, 14},  {6, 9},
};

// Test 16
static struct Point interval_16[] = {
    SOURCE, EMPTY,  SOURCE, TARGET, SOURCE, SOURCE, SOURCE, EMPTY,  SOURCE,
    TARGET, TARGET, SOURCE, EMPTY,  SOURCE, TARGET, TARGET, TARGET, SOURCE,
    SOURCE, SOURCE, SOURCE, EMPTY,  SOURCE, SOURCE, SOURCE,
};
static struct Pair mapping_16[] = {
    {2, 3}, {8, 9}, {11, 10}, {13, 14}, {17, 15}, {18, 16},
};

// Test 17
static struct Point interval_17[] = {
    SOURCE, EMPTY, TARGET, EMPTY,  SOURCE, EMPTY,  SOURCE, TARGET, TARGET,
    SOURCE, EMPTY, SOURCE, TARGET, TARGET, TARGET, SOURCE, EMPTY,  SOURCE,
};
static struct Pair mapping_17[] = {
    {0, 2}, {6, 7}, {9, 8}, {11, 12}, {15, 13}, {17, 14},
};

// Test 18
static struct Point interval_18[] = {
    SOURCE, EMPTY,  EMPTY,  SOURCE, EMPTY,  TARGET, TARGET, TARGET,
    EMPTY,  TARGET, EMPTY,  EMPTY,  SOURCE, TARGET, TARGET, EMPTY,
    TARGET, SOURCE, SOURCE, EMPTY,  SOURCE, EMPTY,  EMPTY,  SOURCE,
    SOURCE, SOURCE, SOURCE, EMPTY,  TARGET, EMPTY,  SOURCE, EMPTY,
};
static struct Pair mapping_18[] = {
    {0, 5}, {3, 6}, {12, 7}, {17, 9}, {18, 13}, {20, 14}, {23, 16}, {26, 28},
};

// Test 19
static struct Point interval_19[] = {BOTH};
static struct Pair mapping_19[] = {{0, 0}};

// Test 20
static struct Point interval_20[] = {BOTH, BOTH, BOTH};
static struct Pair mapping_20[] = {{0, 0}, {1, 1}, {2, 2}};

// Test 21
static struct Point interval_21[] = {
    EMPTY, BOTH, TARGET, BOTH,   EMPTY,  SOURCE, EMPTY,  SOURCE,
    EMPTY, BOTH, TARGET, SOURCE, SOURCE, EMPTY,  TARGET, EMPTY,
};
static struct Pair mapping_21[] = {
    {1, 1}, {3, 2}, {5, 3}, {9, 9}, {11, 10}, {12, 14},
};

// Test 22
static struct Point interval_22[] = {
    BOTH, SOURCE, SOURCE, TARGET, BOTH, EMPTY, BOTH, BOTH, SOURCE,
};
static struct Pair mapping_22[] = {
    {0, 0}, {2, 3}, {4, 4}, {6, 6}, {7, 7},
};

// Test 23
static struct Point interval_23[] = {
    SOURCE, SOURCE, SOURCE, SOURCE, SOURCE, SOURCE, SOURCE,
    SOURCE, SOURCE, SOURCE, SOURCE, SOURCE, SOURCE, SOURCE,
    SOURCE, SOURCE, SOURCE, SOURCE, SOURCE, SOURCE, BOTH,
};
static struct Pair mapping_23[] = {{20, 20}};

// Test 24
static struct Point interval_24[] = {
    TARGET, BOTH, SOURCE, TARGET, BOTH,   SOURCE, SOURCE,
    SOURCE, BOTH, TARGET, SOURCE, SOURCE, SOURCE, BOTH,
};
static struct Pair mapping_24[] = {
    {1, 0}, {2, 1}, {4, 3}, {5, 4}, {8, 8}, {10, 9}, {13, 13},
};

// Test 25
static struct Point interval_25[] = {
    BOTH,   TARGET, BOTH, SOURCE, SOURCE, SOURCE, SOURCE, SOURCE,
    SOURCE, SOURCE, BOTH, SOURCE, SOURCE, BOTH,   TARGET, BOTH,
};
static struct Pair mapping_25[] = {
    {0, 0}, {2, 1}, {3, 2}, {10, 10}, {12, 13}, {13, 14}, {15, 15},
};

// Test 26
static struct Point interval_26[] = {
    SOURCE, SOURCE, EMPTY, TARGET, BOTH, SOURCE,
};
static struct Pair mapping_26[] = {{1, 3}, {4, 4}};

const struct LinearSolverTestCase linear_solver_test_cases[] = {
    NEW_LINEAR_SOLVER_TEST_CASE(interval_1, mapping_1),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_2, mapping_2),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_3, mapping_3),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_4, mapping_4),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_5, mapping_5),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_6, mapping_6),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_7, mapping_7),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_8, mapping_8),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_9, mapping_9),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_10, mapping_10),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_11, mapping_11),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_12, mapping_12),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_13, mapping_13),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_14, mapping_14),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_15, mapping_15),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_16, mapping_16),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_17, mapping_17),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_18, mapping_18),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_19, mapping_19),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_20, mapping_20),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_21, mapping_21),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_22, mapping_22),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_23, mapping_23),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_24, mapping_24),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_25, mapping_25),
    NEW_LINEAR_SOLVER_TEST_CASE(interval_26, mapping_26),
};

const int linear_solver_test_cases_num =
    sizeof(linear_solver_test_cases) / sizeof(linear_solver_test_cases[0]);
