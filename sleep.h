#ifndef SLEEP_H
#define SLEEP_H
#include <stdint.h>
#include <stdlib.h>

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)
void sleep_milliseconds(uint32_t millis);

void sleep_nanoseconds(uint32_t nanos);

void set_max_priority(void);
void set_default_priority(void);
#endif
