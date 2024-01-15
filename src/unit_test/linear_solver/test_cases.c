#include <stdlib.h>

#include "test_cases.h"

#define INTERVAL(points_array)                                                 \
  &(struct Interval) {                                                         \
    .array = points_array,                                                     \
    .length = sizeof(points_array) / sizeof(points_array[0]),                  \
  }

#define MAPPING(pairs_array)                                                   \
  &(struct Mapping) {                                                          \
    .pairs = pairs_array,                                                      \
    .pair_count = sizeof(pairs_array) / sizeof(pairs_array[0]),                \
  }

// Test 1
static struct Point interval_1[] = {
    EMPTY,  EMPTY,  SOURCE, TARGET, TARGET, SOURCE,
    SOURCE, SOURCE, EMPTY,  TARGET, TARGET, EMPTY,
};
static struct Pair pairs_1[] = {{2, 3}, {5, 4}, {6, 9}, {7, 10}};

static struct LinearSolverTestCase test_1 = {
    .input = INTERVAL(interval_1),
    .expected_output = MAPPING(pairs_1),
};

// Test 2
static struct Point interval_2[] = {
    EMPTY,  EMPTY,  SOURCE, SOURCE, TARGET, TARGET, TARGET,
    SOURCE, TARGET, SOURCE, SOURCE, EMPTY,  TARGET, SOURCE,
    SOURCE, SOURCE, SOURCE, EMPTY,  TARGET, TARGET, EMPTY,
};
static struct Pair pairs_2[] = {
    {13, 12}, {3, 5}, {7, 6}, {16, 19}, {9, 8}, {2, 4}, {15, 18},
};

static struct LinearSolverTestCase test_2 = {
    .input = INTERVAL(interval_2),
    .expected_output = MAPPING(pairs_2),
};

// Test 3
static struct Point interval_3[] = {
    EMPTY,  TARGET, TARGET, EMPTY,  EMPTY,  SOURCE, SOURCE,
    SOURCE, SOURCE, TARGET, EMPTY,  SOURCE, SOURCE, TARGET,
    SOURCE, TARGET, TARGET, TARGET, SOURCE, SOURCE, EMPTY,
};
static struct Pair pairs_3[] = {
    {5, 1}, {6, 2}, {8, 9}, {12, 13}, {14, 15}, {18, 16}, {19, 17},
};

static struct LinearSolverTestCase test_3 = {
    .input = INTERVAL(interval_3),
    .expected_output = MAPPING(pairs_3),
};

// Test 4
static struct Point interval_4[] = {
    SOURCE, TARGET, SOURCE, TARGET, SOURCE, TARGET, SOURCE,
    TARGET, SOURCE, TARGET, SOURCE, TARGET, SOURCE, TARGET,
};
static struct Pair pairs_4[] = {
    {0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9}, {10, 11}, {12, 13},
};

static struct LinearSolverTestCase test_4 = {
    .input = INTERVAL(interval_4),
    .expected_output = MAPPING(pairs_4),
};

// Test 5
static struct Point interval_5[] = {
    TARGET, SOURCE, SOURCE, SOURCE, TARGET, SOURCE,
    TARGET, TARGET, TARGET, SOURCE, SOURCE,
};
static struct Pair pairs_5[] = {
    {1, 0}, {3, 4}, {5, 6}, {9, 7}, {10, 8},
};

static struct LinearSolverTestCase test_5 = {
    .input = INTERVAL(interval_5),
    .expected_output = MAPPING(pairs_5),
};

// Test 6
static struct Point interval_6[] = {
    TARGET, TARGET, SOURCE, SOURCE, SOURCE, SOURCE, SOURCE,
};
static struct Pair pairs_6[] = {{2, 0}, {3, 1}};

static struct LinearSolverTestCase test_6 = {
    .input = INTERVAL(interval_6),
    .expected_output = MAPPING(pairs_6),
};

// Test 7
static struct Point interval_7[] = {
    TARGET, TARGET, EMPTY, EMPTY, EMPTY, SOURCE, SOURCE,
};
static struct Pair pairs_7[] = {{5, 0}, {6, 1}};

static struct LinearSolverTestCase test_7 = {
    .input = INTERVAL(interval_7),
    .expected_output = MAPPING(pairs_7),
};

// Test 8
static struct Point interval_8[] = {SOURCE, SOURCE, TARGET, TARGET};
static struct Pair pairs_8[] = {{0, 2}, {1, 3}};

static struct LinearSolverTestCase test_8 = {
    .input = INTERVAL(interval_8),
    .expected_output = MAPPING(pairs_8),
};

// Test 9
static struct Point interval_9[] = {SOURCE, SOURCE};
static struct Pair pairs_9[] = {};

static struct LinearSolverTestCase test_9 = {
    .input = INTERVAL(interval_9),
    .expected_output = MAPPING(pairs_9),
};

// Test 10
static struct Point interval_10[] = {EMPTY};
static struct Pair pairs_10[] = {};

static struct LinearSolverTestCase test_10 = {
    .input = INTERVAL(interval_10),
    .expected_output = MAPPING(pairs_10),
};

// Test 11
static struct Point interval_11[] = {};
static struct Pair pairs_11[] = {};

static struct LinearSolverTestCase test_11 = {
    .input = INTERVAL(interval_11),
    .expected_output = MAPPING(pairs_11),
};

// Test 12
static struct Point interval_12[] = {TARGET};

static struct LinearSolverTestCase test_12 = {
    .input = INTERVAL(interval_12),
    .expected_output = NULL,
};

// Test 13
static struct Point interval_13[] = {
    TARGET, EMPTY,  SOURCE, EMPTY, TARGET, SOURCE,
    TARGET, TARGET, EMPTY,  EMPTY, SOURCE,
};

static struct LinearSolverTestCase test_13 = {
    .input = INTERVAL(interval_13),
    .expected_output = NULL,
};

// Test 14
static struct Point interval_14[] = {TARGET, EMPTY, TARGET};

static struct LinearSolverTestCase test_14 = {
    .input = INTERVAL(interval_14),
    .expected_output = NULL,
};

// Test 15
static struct Point interval_15[] = {
    TARGET, SOURCE, SOURCE, TARGET, SOURCE, SOURCE, SOURCE, SOURCE, SOURCE,
    TARGET, SOURCE, SOURCE, SOURCE, EMPTY,  TARGET, EMPTY,  TARGET, TARGET,
    TARGET, EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  SOURCE, SOURCE, TARGET,
    TARGET, SOURCE, TARGET, EMPTY,  TARGET, EMPTY,
};
static struct Pair pairs_15[] = {
    {1, 0},   {2, 3},   {28, 31}, {25, 29}, {24, 27}, {12, 26},
    {11, 18}, {10, 17}, {8, 16},  {7, 14},  {6, 9},
};

static struct LinearSolverTestCase test_15 = {
    .input = INTERVAL(interval_15),
    .expected_output = MAPPING(pairs_15),
};

// Test 16
static struct Point interval_16[] = {
    SOURCE, EMPTY,  SOURCE, TARGET, SOURCE, SOURCE, SOURCE, EMPTY,  SOURCE,
    TARGET, TARGET, SOURCE, EMPTY,  SOURCE, TARGET, TARGET, TARGET, SOURCE,
    SOURCE, SOURCE, SOURCE, EMPTY,  SOURCE, SOURCE, SOURCE,
};
static struct Pair pairs_16[] = {
    {2, 3}, {8, 9}, {11, 10}, {13, 14}, {17, 15}, {18, 16},
};

static struct LinearSolverTestCase test_16 = {
    .input = INTERVAL(interval_16),
    .expected_output = MAPPING(pairs_16),
};

// Test 17
static struct Point interval_17[] = {
    SOURCE, EMPTY, TARGET, EMPTY,  SOURCE, EMPTY,  SOURCE, TARGET, TARGET,
    SOURCE, EMPTY, SOURCE, TARGET, TARGET, TARGET, SOURCE, EMPTY,  SOURCE,
};
static struct Pair pairs_17[] = {
    {0, 2}, {6, 7}, {9, 8}, {11, 12}, {15, 13}, {17, 14},
};

static struct LinearSolverTestCase test_17 = {
    .input = INTERVAL(interval_17),
    .expected_output = MAPPING(pairs_17),
};

// Test 18
static struct Point interval_18[] = {
    SOURCE, EMPTY,  EMPTY,  SOURCE, EMPTY,  TARGET, TARGET, TARGET,
    EMPTY,  TARGET, EMPTY,  EMPTY,  SOURCE, TARGET, TARGET, EMPTY,
    TARGET, SOURCE, SOURCE, EMPTY,  SOURCE, EMPTY,  EMPTY,  SOURCE,
    SOURCE, SOURCE, SOURCE, EMPTY,  TARGET, EMPTY,  SOURCE, EMPTY,
};
static struct Pair pairs_18[] = {
    {0, 5}, {3, 6}, {12, 7}, {17, 9}, {18, 13}, {20, 14}, {23, 16}, {26, 28},
};

static struct LinearSolverTestCase test_18 = {
    .input = INTERVAL(interval_18),
    .expected_output = MAPPING(pairs_18),
};

// Test 19
static struct Point interval_19[] = {BOTH};
static struct Pair pairs_19[] = {{0, 0}};

static struct LinearSolverTestCase test_19 = {
    .input = INTERVAL(interval_19),
    .expected_output = MAPPING(pairs_19),
};

// Test 20
static struct Point interval_20[] = {BOTH, BOTH, BOTH};
static struct Pair pairs_20[] = {{0, 0}, {1, 1}, {2, 2}};

static struct LinearSolverTestCase test_20 = {
    .input = INTERVAL(interval_20),
    .expected_output = MAPPING(pairs_20),
};

// Test 21
static struct Point interval_21[] = {
    EMPTY, BOTH, TARGET, BOTH,   EMPTY,  SOURCE, EMPTY,  SOURCE,
    EMPTY, BOTH, TARGET, SOURCE, SOURCE, EMPTY,  TARGET, EMPTY,
};
static struct Pair pairs_21[] = {
    {1, 1}, {3, 2}, {5, 3}, {9, 9}, {11, 10}, {12, 14},
};

static struct LinearSolverTestCase test_21 = {
    .input = INTERVAL(interval_21),
    .expected_output = MAPPING(pairs_21),
};

// Test 22
static struct Point interval_22[] = {
    BOTH, SOURCE, SOURCE, TARGET, BOTH, EMPTY, BOTH, BOTH, SOURCE,
};
static struct Pair pairs_22[] = {
    {0, 0}, {2, 3}, {4, 4}, {6, 6}, {7, 7},
};

static struct LinearSolverTestCase test_22 = {
    .input = INTERVAL(interval_22),
    .expected_output = MAPPING(pairs_22),
};

// Test 23
static struct Point interval_23[] = {
    SOURCE, SOURCE, SOURCE, SOURCE, SOURCE, SOURCE, SOURCE,
    SOURCE, SOURCE, SOURCE, SOURCE, SOURCE, SOURCE, SOURCE,
    SOURCE, SOURCE, SOURCE, SOURCE, SOURCE, SOURCE, BOTH,
};
static struct Pair pairs_23[] = {{20, 20}};

static struct LinearSolverTestCase test_23 = {
    .input = INTERVAL(interval_23),
    .expected_output = MAPPING(pairs_23),
};

// Test 24
static struct Point interval_24[] = {
    TARGET, BOTH, SOURCE, TARGET, BOTH,   SOURCE, SOURCE,
    SOURCE, BOTH, TARGET, SOURCE, SOURCE, SOURCE, BOTH,
};
static struct Pair pairs_24[] = {
    {1, 0}, {2, 1}, {4, 3}, {5, 4}, {8, 8}, {10, 9}, {13, 13},
};

static struct LinearSolverTestCase test_24 = {
    .input = INTERVAL(interval_24),
    .expected_output = MAPPING(pairs_24),
};

// Test 25
static struct Point interval_25[] = {
    BOTH,   TARGET, BOTH, SOURCE, SOURCE, SOURCE, SOURCE, SOURCE,
    SOURCE, SOURCE, BOTH, SOURCE, SOURCE, BOTH,   TARGET, BOTH,
};
static struct Pair pairs_25[] = {
    {0, 0}, {2, 1}, {3, 2}, {10, 10}, {12, 13}, {13, 14}, {15, 15},
};

static struct LinearSolverTestCase test_25 = {
    .input = INTERVAL(interval_25),
    .expected_output = MAPPING(pairs_25),
};

// Test 26
static struct Point interval_26[] = {
    SOURCE, SOURCE, EMPTY, TARGET, BOTH, SOURCE,
};
static struct Pair pairs_26[] = {{1, 3}, {4, 4}};

static struct LinearSolverTestCase test_26 = {
    .input = INTERVAL(interval_26),
    .expected_output = MAPPING(pairs_26),
};

const struct LinearSolverTestCase *linear_solver_test_cases[] = {
    &test_1,  &test_2,  &test_3,  &test_4,  &test_5,  &test_6,  &test_7,
    &test_8,  &test_9,  &test_10, &test_11, &test_12, &test_13, &test_14,
    &test_15, &test_16, &test_17, &test_18, &test_19, &test_20, &test_21,
    &test_22, &test_23, &test_24, &test_25, &test_26,
};

const int linear_solver_test_cases_num =
    sizeof(linear_solver_test_cases) / sizeof(linear_solver_test_cases[0]);
