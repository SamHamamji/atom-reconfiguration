#include <math.h>
#include <stdlib.h>

#include "./integer_length.h"

int integer_length(int num) {
  if (num == 0) {
    return 1;
  }

  return (int)log10((double)abs(num)) + 1 + (int)(num < 0);
}
