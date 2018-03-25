
#ifndef DH22_H
#define DH22_H

#include "sample.h"

#define DHT_PULSES  41
#define DHT_TIMEOUT 60000

int dh22_read(uint32_t pin, float* temp, float* hum);
int dh22_read_multi(uint32_t pin, uint32_t samples, sample_float *temp, sample_float* hum);
#endif
