#include <stdlib.h>

#include "mapping.h"

void mapping_free(struct Mapping *mapping) {
  if (mapping == NULL) {
    return;
  }
  free(mapping->pairs);
  free(mapping);
}

inline static bool pair_equals(const struct Pair a, const struct Pair b) {
  return (a.source == b.source) && (a.target == b.target);
}

bool mapping_equals(const struct Mapping *a, const struct Mapping *b) {
  if (a == NULL || b == NULL) {
    return a == b;
  }

  if (a->pair_count != b->pair_count) {
    return false;
  }

  bool *pair_is_matched = calloc(a->pair_count, sizeof(bool));

  for (int i = 0; i < a->pair_count; i++) {
    bool found = false;
    for (int j = 0; j < b->pair_count; j++) {
      if (!pair_is_matched[j] && pair_equals(a->pairs[i], b->pairs[j])) {
        pair_is_matched[j] = true;
        found = true;
        break;
      }
    }
    if (!found) {
      free(pair_is_matched);
      return false;
    }
  }

  free(pair_is_matched);
  return true;
}

struct Mapping *mapping_get_null() {
  struct Mapping *mapping = malloc(sizeof(struct Mapping));
  mapping->pair_count = 0;
  mapping->pairs = NULL;
  return mapping;
}
