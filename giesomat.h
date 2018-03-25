
#ifndef GIESOMAT_H
#define GIESOMAT_H

#include "sample.h"
#define MAX_FREQUENCY
#define MIN_FREQUENCY
#define GIESSOMAT_TIMEOUT 60000
int giesomat_read(uint32_t pin, float* percent, uint32_t *raw);
int giesomat_read_multi(uint32_t pin, uint32_t samples, sample_float *percent, sample_uint32_t* raw);
#endif
