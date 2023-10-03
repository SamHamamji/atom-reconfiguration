#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "mapping.h"

static int max_mapping_index(const struct Mapping *mapping) {
  unsigned int max = 0;
  for (unsigned int i = 0; i < mapping->pair_count; i++) {
    if (mapping->pairs[i].target > max) {
      max = mapping->pairs[i].target;
    }
    if (mapping->pairs[i].source > max) {
      max = mapping->pairs[i].source;
    }
  }
  return max;
}

static char *mapping_to_string(const struct Mapping *mapping) {
  if (mapping->pair_count == 0) {
    char *string = malloc(6);
    sprintf(string, (mapping->pairs == NULL) ? "NULL" : "EMPTY");
    return string;
  }
  int max_index_length = (int)log10((double)max_mapping_index(mapping)) + 1;
  const int line_length = 5 + max_index_length * 2;
  char *string = malloc(line_length * mapping->pair_count * sizeof(char));
  for (unsigned int i = 0; i < mapping->pair_count; i++) {
    sprintf(&string[line_length * i], "%-*d -> %*d", max_index_length,
            mapping->pairs[i].source, max_index_length,
            mapping->pairs[i].target);
    string[line_length * i + line_length - 1] = '\n';
  }
  string[line_length * mapping->pair_count - 1] = '\0';
  return string;
}

void mapping_print(const struct Mapping *mapping) {
  char *string = mapping_to_string(mapping);
  printf("%s\n", string);
  free(string);
}
