
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

#include "config.h"
#include "gpio.h"
#include "giesomat.h"
#include "sleep.h"

int giesomat_read(uint32_t pin, float* percent, uint32_t *raw)
{
    int rt = 0;
    uint32_t count = 0;
    uint32_t pulse_time = 0;

    set_max_priority();

    pi_set_input(pin);

    struct timeval time_start;
    struct timeval time_end;
    struct timeval deltatime;

    deltatime.tv_sec = 1;
    deltatime.tv_usec = 0;

    gettimeofday(&time_start, NULL);
    timeradd(&time_start, &deltatime, &time_end);

    while (true) {
        uint32_t p = 0;

        while (!GET_GPIO(pin)) { }
        while (GET_GPIO(pin)) { }

        pulse_time +=1;
        gettimeofday(&time_start, NULL);
        if (timercmp(&time_start, &time_end, >)) {
            break;
        }
    }

    printf("giesomat pin %d %d \n",pin , pulse_time);

    // calculate the data by distance and scale
    if (0)
    {
    }

error:
    set_default_priority();
    return rt;
}

int giesomat_read_multi(uint32_t pin,
                        uint32_t samples,
                        sample_float *percent,
                        sample_uint32_t *raw)
{
    int rt = 0;
    int s = 0;
    float *p = NULL;
    uint32_t *r = NULL;

    p = malloc(sizeof(float) * samples);
    if (p == NULL) {
        rt = -ENOMEM;
        goto end;
    }
    r = malloc(sizeof(uint32_t) * samples);
    if (r == NULL) {
        rt = -ENOMEM;
        goto end;
    }

    // read samples
    for (int s = 0; s < samples; ++s) {
        int err;
        while (1) {
            err = giesomat_read(pin, &p[s], &r[s]);

            sleep_milliseconds(500);
            if (err >= 0)
                break;
        }
    }

    // sort
    qsort(p, samples, sizeof(float), comp_float);
    qsort(r, samples, sizeof(float), comp_uint32_t);

    // store data
    percent->min = p[0];
    percent->mean = p[samples/2];
    percent->max = p[samples-1];

    raw->min = r[0];
    raw->mean = r[samples/2];
    raw->max = r[samples-1];

    rt = 0;
end:
    if (p != NULL)
        free(p);
    if (r != NULL)
        free(r);
    return rt;
}
