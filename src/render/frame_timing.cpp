#include "frame_timing.hpp"
#include <ctime>

static constexpr f64 TARGET_FRAME_TIME = 1.0 / FPS;

static f64 frame_start_time;

void frame_timer_start() {
  timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  frame_start_time = now.tv_sec + now.tv_nsec * 0.000000001;
}

void frame_timer_end() {
  timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  f64 frame_end_time = now.tv_sec + now.tv_nsec * 0.000000001;
  f64 frame_elapsed_time = frame_end_time - frame_start_time;
  f64 remaining_seconds = TARGET_FRAME_TIME - frame_elapsed_time;
  if (remaining_seconds > 0.0) {
    timespec ts;
    ts.tv_sec = (int)remaining_seconds;
    ts.tv_nsec = (int)((remaining_seconds - ts.tv_sec - 0.001) * 1000000000);
    nanosleep(&ts, 0);
  }
}
