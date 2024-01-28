#include <stdlib.h>

#include "mapping.h"

struct Mapping *mapping_new(int pair_count) {
  struct Mapping *mapping = malloc(sizeof(struct Mapping));
  *mapping = (struct Mapping){
      .pairs = malloc(pair_count * sizeof(struct Pair)),
      .pair_count = pair_count,
  };
  return mapping;
}

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
