#include "./point.h"

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define BLUE "\x1b[34m"
#define RESET "\x1b[0m"

char point_to_char(const struct Point point) {
  return point.is_source ? (point.is_target ? 'B' : 'S')
                         : (point.is_target ? 'T' : '.');
}

char *point_to_string(const struct Point point) {
  return point.is_source ? (point.is_target ? GREEN "B" RESET : BLUE "S" RESET)
                         : (point.is_target ? RED "T" RESET : ".");
}
