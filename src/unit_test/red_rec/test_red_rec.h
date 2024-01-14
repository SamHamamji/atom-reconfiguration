#pragma once

#include <stdbool.h>

struct RedRecUnitTestConfig {
  int max_height;
  int min_height;
  int max_width;
  int min_width;
  int test_case_num;
};

bool test_red_rec(const struct RedRecUnitTestConfig *config);
