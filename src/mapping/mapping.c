#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mapping.h"

void mapping_free(struct Mapping *mapping) {
  free(mapping->pairs);
  free(mapping);
}

static int pair_equals(const struct Pair a, const struct Pair b) {
  int diff = a.target - b.target;
  return !((diff == 0) ? a.source - b.source : diff);
}

int mapping_equals(const struct Mapping *a, const struct Mapping *b) {
  if (a->pair_count != b->pair_count) {
    return 0;
  }

  int used[a->pair_count];
  memset(used, 0, sizeof(used));

  for (int i = 0; i < a->pair_count; i++) {
    int found = 0;
    for (int j = 0; j < b->pair_count; j++) {
      if (!used[j] && pair_equals(a->pairs[i], b->pairs[j])) {
        used[j] = 1;
        found = 1;
        break;
      }
    }
    if (!found) {
      return 0;
    }
  }

  return 1;
}

struct Mapping *mapping_get_null() {
  struct Mapping *mapping = malloc(sizeof(struct Mapping));
  mapping->pair_count = 0;
  mapping->pairs = NULL;
  return mapping;
}
