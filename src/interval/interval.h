#pragma once

typedef enum {
  EMPTY,
  TARGET,
  SOURCE,
} Point;

struct Interval {
  Point *array;
  int size;
};

struct Interval *new_interval(const Point points[], int size);
void interval_free(struct Interval *interval);

char *interval_to_string(struct Interval *interval);
void interval_print(struct Interval *interval);
