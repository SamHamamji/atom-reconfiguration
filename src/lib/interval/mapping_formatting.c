#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "mapping.h"

static int integer_length(int num) {
  return (int)log10((double)abs(num)) + 1 + (num <= 0);
}

static int max_mapping_index_length(const struct Mapping *mapping) {
  int max = 1;
  for (int i = 0; i < mapping->pair_count; i++) {
    int target_length = integer_length(mapping->pairs[i].target);
    int source_length = integer_length(mapping->pairs[i].source);
    if (source_length > max) {
      max = source_length;
    }
    if (target_length > max) {
      max = target_length;
    }
  }
  return max;
}

static char *mapping_to_string(const struct Mapping *mapping) {
  if (mapping->pair_count == 0) {
    char *string = malloc(6);
    if (string)
      sprintf(string, (mapping->pairs == NULL) ? "NULL" : "EMPTY");
    return string;
  }
  const int max_index_length = max_mapping_index_length(mapping);
  const int line_length = 5 + max_index_length * 2;

  char *string = calloc(mapping->pair_count, line_length * sizeof(char));
  for (int i = 0; i < mapping->pair_count; i++) {
    sprintf(&string[line_length * i], "%-*d -> %*d", max_index_length,
            mapping->pairs[i].source, max_index_length,
            mapping->pairs[i].target);
    string[line_length * i + line_length - 1] = '\n';
  }
  string[line_length * mapping->pair_count - 1] = '\0';
  return string;
}

void mapping_print(const struct Mapping *mapping) {
  if (mapping == NULL) {
    printf("NULL mapping\n");
    return;
  }
  if (mapping->pair_count == 0) {
    printf("Empty mapping\n");
    return;
  }
  char *string = mapping_to_string(mapping);
  printf("%s\n", string);
  free(string);
}
