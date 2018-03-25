#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "config.h"

#include "config.h"
#include "gpio.h"
#include "sleep.h"
#include "dh22.h"
#include "debug.h"

int dh22_read(uint32_t pin, float* temp, float* hum)
{
    int rt = 0;
    uint32_t pulse_time[DHT_PULSES];
    uint32_t p;
    uint32_t count = 0;

    memset(pulse_time, 0, sizeof(pulse_time));

    pi_set_output(pin);
    set_max_priority();

    pi_set_high(pin);
    sleep_milliseconds(30);
    pi_set_low(pin);
    sleep_milliseconds(20);

    pi_set_input(pin);
    sleep_nanoseconds(1);

    while (GET_GPIO(pin)) {
        if (unlikely(++count >= DHT_TIMEOUT)) {
#ifdef ERROR
            printf("dh22 pin %d timeout 1 idx %d\n",pin ,p);
#endif
            goto error;
        }
    }

    for (int i=0; i < DHT_PULSES ; ++i) {
        p=0;
        while (!GET_GPIO(pin)) {
            if (unlikely(++p >= DHT_TIMEOUT)) {
#ifdef ERROR
                printf("dh22 pin %d timeout 3 idx %d\n",pin, p);
#endif
                goto error;
            }
        }
        p=0;
        while (GET_GPIO(pin)) {
            if (unlikely(++p >= DHT_TIMEOUT)) {
#ifdef ERROR
                printf("dh22 pin %d timeout 3 idx %d\n",pin ,p);
#endif
                goto error;
            }
        }
        pulse_time[i] = p;
    }

    // zero is 28-30 us one is 70us
    // therfore the ratio is 1:2.5
    // the assumption to make a threshold over the average
    // is wrong because it asumes a equal distribution of one/zeros.
    //
    // Instead calculate distances to min and max and scale them by
    // the factor 1:2.5.
    // Then take the min distance value

    uint32_t pulse_max = pulse_time[0];
    uint32_t pulse_min = pulse_time[0];

    // calculate the min/max
    {
        for (int i=1; i < DHT_PULSES; ++i) {

            if (pulse_min > pulse_time[i])
                pulse_min = pulse_time[i];

            if (pulse_max < pulse_time[i])
                pulse_max = pulse_time[i];
        }
    }
#ifdef DEBUG
    printf("dh22 pin %d min %d max %d \n",pin ,pulse_min, pulse_max);
#endif

    // calculate the data by distance and scale
    uint8_t data[5] = {0};
    {
        uint8_t bit_pos = 7;
        uint8_t index = 0;

        uint32_t pulse_distance_min = 0;
        uint32_t pulse_distance_max = 0;

        for (uint32_t p = 1; p < DHT_PULSES; ++p) {
            bool bit = false;
            pulse_distance_min = pulse_time[p] - pulse_min;
            pulse_distance_max = pulse_max - pulse_time[p]; //< we are interested
                                                            //< in unsigned distances
                                                            //< so swap for max

            // scale min to match the 1:2.5 factor for max
            // but stay in integer type
            pulse_distance_min = (pulse_distance_min<<1); // + (pulse_distance_min/2);

            // store the bit into the data array
            if (pulse_distance_min < pulse_distance_max) {
                // distance is smaller for min so its a zero
                // nothing to do since data is already zero
                bit = false;
            } else {
                // distance is smaller for max so its a one
                data[index] |= (1 << bit_pos);
                bit = true;
            }
#ifdef DEBUG
            printf("bit%d / %d %d: % 5d : % 5d % 5d bit %d\n", p, bit_pos, index, pulse_time[p], pulse_distance_min, pulse_distance_max , bit);
#endif

            if (bit_pos == 0) {
                // switch to the next byte
                bit_pos = 7;
                index += 1;
            } else {
                // switch to the next bit
                bit_pos -=1;
            }
        }
    }

#ifdef DEBUG
    printf("dh22 pin %d Data: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x bits ",pin, data[0], data[1], data[2], data[3], data[4]);
    printBits(sizeof(data), data);
    printf("\n");
#endif

    // check for crc error
    if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
#ifdef ERROR
        printf("dh22 pin %d: crc error\n", pin);
#endif
        //rt = -EINVAL;
        //goto error;
    }


    uint16_t t = 0;
    uint16_t h = 0;

    // Calculate humidity
    h = data[0];
    h = h << 8;
    h = h + data[1];

    *hum = h / 10.0f;
#ifdef DEBUG
    printf("dh22 pin %d: hum %d %f \n",pin, h, *hum);
#endif

    // calculate temp
    t = data[2] & 0x7F;
    t = t << 8;
    t = t + data[3];
    *temp = t / 10.0f;

    // highest bit is signed (-/+)
    uint8_t signed_bit = data[2] >> 7;
    if (signed_bit) {
        *temp *= -1.0;
    }
#ifdef DEBUG
    printf("dh22 pin %d: temp s%d %d %f \n", pin, signed_bit, t, *temp);
#endif

    rt = 0;

error:

    set_default_priority();
    return rt;
}


int dh22_read_multi(uint32_t pin,
                    uint32_t samples,
                    sample_float *temp,
                    sample_float *hum)
{
    int rt = 0;
    int s = 0;
    float *t = NULL;
    float *h = NULL;

    t = malloc(sizeof(float) * samples);
    if (t == NULL) {
        rt = -ENOMEM;
        goto end;
    }
    h = malloc(sizeof(float) * samples);
    if (h == NULL) {
        rt = -ENOMEM;
        goto end;
    }

    // read samples
    for ( int s = 0; s < samples; ++s) {
        int err;
        while (1) {
            err = dh22_read(pin, &t[s], &h[s]);

            // datasheet allows only read every 2 seconds
            sleep_milliseconds(500);

            if (err >= 0)
                break;
        }
    }

    // sort
    qsort(t, samples, sizeof(float), comp_float);
    qsort(h, samples, sizeof(float), comp_float);

    // store data
    temp->min = t[0];
    temp->mean = t[samples/2];
    temp->max = t[samples-1];

    hum->min = h[0];
    hum->mean = h[samples/2];
    hum->max = h[samples-1];

    rt = 0;
end:
    if (t != NULL)
        free(t);
    if (h != NULL)
        free(h);
    return rt;
}
