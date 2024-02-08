#include "./range.h"
#include "./max_min.h"
#include <stdlib.h>

inline static int get_range_start(int range_index, int range_num,
                                  int total_length) {
  int remaining_length = total_length % range_num;
  return range_index * (total_length / range_num) +
         min(range_index, remaining_length);
}

inline struct Range get_range(int range_index, int range_num,
                              int total_length) {
  return (struct Range){
      .start = get_range_start(range_index, range_num, total_length),
      .exclusive_end =
          get_range_start(range_index + 1, range_num, total_length),
  };
}

inline int get_range_index(int index, int range_num, int total_length) {
  int subinterval_length = total_length / range_num;
  int remaining_length = total_length % range_num;
  int potential_thread_index = index / (subinterval_length + 1);

  if (potential_thread_index < remaining_length) {
    return potential_thread_index;
  } else {
    return (index - remaining_length) / subinterval_length;
  }
}

int get_random_int_in_range(struct Range range) {
  return range.start + rand() % max(1, range.exclusive_end - range.start);
}
