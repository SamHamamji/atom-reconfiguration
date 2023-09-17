#include <stdio.h>
#include <stdlib.h>

#include "mapping.h"

void mapping_free(struct Mapping *mapping) {
  free(mapping->pairs);
  free(mapping);
}
