

#ifndef SAMPLE_H
#define SAMPLE_H
#include "stdint.h"
#include "stdbool.h"

#define sample(type) \
typedef struct       \
{                    \
    type min;        \
    type mean;       \
    type max;        \
} sample_##type

sample(uint32_t);
sample(uint16_t);
sample(uint8_t);

sample(int32_t);
sample(int16_t);
sample(int8_t);

sample(float);
sample(double);

#define compare_H(type)                                         \
int comp_##type (const void * elem1, const void * elem2);

#define compare_C(type)                                         \
int comp_##type (const void * elem1, const void * elem2)        \
{                                                               \
    type f = *(type *)elem1;                                    \
    type s = *(type *)elem2;                                    \
                                                                \
    if (f > s)                                                  \
        return  1;                                              \
    else if (f < s)                                             \
        return -1;                                              \
    else                                                        \
        return 0;                                               \
}

compare_H(uint32_t)
compare_H(uint16_t)
compare_H(uint8_t)

compare_H(int32_t)
compare_H(int16_t)
compare_H(int8_t)

compare_H(float)
compare_H(double)
#endif
