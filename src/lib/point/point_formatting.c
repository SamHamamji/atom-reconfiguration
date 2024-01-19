#include "../../lib/utils/colors.h"
#include "./point.h"

char point_to_char(const struct Point point) {
  return point.is_source ? (point.is_target ? 'B' : 'S')
                         : (point.is_target ? 'T' : '.');
}

char *point_to_string(const struct Point point) {
  return point.is_source ? (point.is_target ? GREEN "B" RESET : BLUE "S" RESET)
                         : (point.is_target ? RED "T" RESET : ".");
}
