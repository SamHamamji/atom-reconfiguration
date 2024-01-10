#pragma once

#include <time.h>

struct Timer {
  struct timespec start_time;
  struct timespec end_time;
};

void timer_start(struct Timer *timer);
void timer_stop(struct Timer *timer);
float timer_get_time(struct Timer *timer);
