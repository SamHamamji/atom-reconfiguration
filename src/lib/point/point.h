#pragma once

#include <stdbool.h>

struct Point {
  bool is_target;
  bool is_source;
};

#define EMPTY                                                                  \
  { .is_target = false, .is_source = false }
#define SOURCE                                                                 \
  { .is_target = false, .is_source = true }
#define TARGET                                                                 \
  { .is_target = true, .is_source = false }
#define BOTH                                                                   \
  { .is_target = true, .is_source = true }

char point_to_char(const struct Point point);
char *point_to_string(const struct Point point);
