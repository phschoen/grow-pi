#include <errno.h>
#include <sched.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>

void sleep_milliseconds(uint32_t millis)
{
    struct timespec sleep;
    sleep.tv_sec = millis / 1000;
    sleep.tv_nsec = (millis % 1000) * 1000000L;
    while (clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep, &sleep) && errno == EINTR);
}
void sleep_nanoseconds(uint32_t nanos)
{
    struct timespec ts;
    ts.tv_sec = nanos/ (1000*1000*1000);
    ts.tv_nsec = nanos;
    do {
        clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, &ts);
    } while (errno == EINTR);
}


void set_max_priority(void) {
  struct sched_param sched;
  memset(&sched, 0, sizeof(sched));
  // Use FIFO scheduler with highest priority for the lowest chance of the kernel context switching.
  sched.sched_priority = sched_get_priority_max(SCHED_FIFO);
  sched_setscheduler(0, SCHED_FIFO, &sched);
}

void set_default_priority(void) {
  struct sched_param sched;
  memset(&sched, 0, sizeof(sched));
  // Go back to default scheduler with default 0 priority.
  sched.sched_priority = 0;
  sched_setscheduler(0, SCHED_OTHER, &sched);
}
