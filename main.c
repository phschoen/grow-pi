#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include <stdlib.h>
#include "gpio.h"
#include "dh22.h"
#include "giesomat.h"


struct {
    sample_float curr_t;
    sample_float curr_h;

    uint32_t pin;
    uint32_t samples;
    const char* description;
} dh22_sensors[] = {{
    .pin = 4,
    .samples = 21,
    .description = "dh22_sensor1",
}, {
    .pin = 17,
    .samples = 21,
    .description = "dh22_sensor2",
}, {
    .pin = 27,
    .samples = 21,
    .description = "dh22_sensor3",
}};

struct {
    sample_float    moisture_percent;
    sample_uint32_t moisture_raw;

    uint32_t pin;
    uint32_t samples;
    const char* description;
} giesomat_sensors[] = {{
    .pin = 14,
    .samples = 21,
    .description = "giesomat_sensor1",
}, {
    .pin = 15,
    .samples = 21,
    .description = "giesomat_sensor2",
}, {
    .pin = 18,
    .samples = 21,
    .description = "giesomat_sensor3",
}};


int main(int argc, char* argv[])
{
    int err = 0;

    err = gpio_init();
    if (err < 0) {
        printf("not init gpio (err %d, errno %s)\n", err, strerror(errno));
        return err;
    }

    struct timeval time;
    char line[1024];
    int offset =0;
    while (1)
    {
        offset = 0;
        offset += snprintf(line + offset, sizeof(line)-offset, "%10lu;", time.tv_sec);
        gettimeofday(&time, NULL);

        // dh22 sensors
        {
            uint32_t l = sizeof(dh22_sensors) / sizeof(dh22_sensors[0]);
            for (uint32_t i = 0; i < l; ++i) {
                err = dh22_read_multi(dh22_sensors[i].pin,
                                      dh22_sensors[i].samples,
                                      &dh22_sensors[i].curr_t,
                                      &dh22_sensors[i].curr_h
                                      );

                offset += snprintf(line + offset,
                                   sizeof(line)-offset,
                                   "% 9.2f;% 9.2f;% 9.2f;% 9.2f;",
                                   dh22_sensors[i].curr_h.mean,
                                   dh22_sensors[i].curr_h.min,
                                   dh22_sensors[i].curr_h.max
                                   );
                offset += snprintf(line + offset,
                                   sizeof(line)-offset,
                                   "% 9.2f;% 9.2f;% 9.2f;% 9.2f;",
                                   dh22_sensors[i].curr_t.mean,
                                   dh22_sensors[i].curr_t.min,
                                   dh22_sensors[i].curr_t.max
                                   );
            }
        }

        // giesomat sensors
        {
            uint32_t l = sizeof(giesomat_sensors) / sizeof(giesomat_sensors[0]);
            for (uint32_t i = 0; i < l; ++i) {
                err = giesomat_read_multi(giesomat_sensors[i].pin,
                                          giesomat_sensors[i].samples,
                                          &giesomat_sensors[i].moisture_percent,
                                          &giesomat_sensors[i].moisture_raw
                                         );
                offset += snprintf(line + offset,
                                   sizeof(line)-offset,
                                   "% 9.2f;% 9.2f;% 9.2f;% 9.2f;",
                                   giesomat_sensors[i].moisture_percent.mean,
                                   giesomat_sensors[i].moisture_percent.min,
                                   giesomat_sensors[i].moisture_percent.max
                                   );
                offset += snprintf(line + offset,
                                   sizeof(line)-offset,
                                   "% 9.2f;% 9.2f;% 9.2f;% 9.2f;",
                                   giesomat_sensors[i].moisture_raw.mean,
                                   giesomat_sensors[i].moisture_raw.min,
                                   giesomat_sensors[i].moisture_raw.max
                                   );

            }
        }

        // temperature sensor
        {
            //TODO implement temperature sensor readings
        }

        // print values
        offset += snprintf(line + offset, sizeof(line)-offset, "\n");
        printf("%s", line);

        // react on values
        {
            //TODO implement watering if needed
            //TODO send msg if needed via telegram/signal
        }
    }

    return 0;
}
