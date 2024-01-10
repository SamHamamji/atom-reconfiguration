#include <stdlib.h>

#include "./timer.h"

inline void timer_start(struct Timer *timer) {
  clock_gettime(CLOCK_MONOTONIC, &timer->start_time);
}

inline void timer_stop(struct Timer *timer) {
  clock_gettime(CLOCK_MONOTONIC, &timer->end_time);
}

inline float timer_get_time(struct Timer *timer) {
  return (timer->end_time.tv_sec - timer->start_time.tv_sec) +
         (timer->end_time.tv_nsec - timer->start_time.tv_nsec) / 1000000000.0;
}
