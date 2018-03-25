#ifndef GPIO_H_
#define GPIO_H_
#include <stdint.h>
extern volatile uint32_t *gpio;

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g)         (*(gpio+((g)/10)) &= ~(7<<(((g)%10)*3)))
#define OUT_GPIO(g)         (*(gpio+((g)/10)) |=  (1<<(((g)%10)*3)))
#define SET_GPIO_ALT(g,a)   (*(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3)))

// sets   bits which are 1 ignores bits which are 0
#define GPIO_SET(g)         (*(gpio+ 7) = 1 << g )
// clears bits which are 1 ignores bits which are 0
#define GPIO_CLR(g)         (*(gpio+10) = 1 << g )

// 0 if LOW, (1<<g) if HIGH
#define GET_GPIO(g)         (*(gpio+13) & (1 << g ))

#define GPIO_PULL           (*(gpio+37)) // Pull up/pull down
#define GPIO_PULLCLK0       (*(gpio+38)) // Pull up/pull down clock

int gpio_init(void);

static inline void pi_set_input(const int gpio_number) {
  // Set GPIO register to 000 for specified GPIO number.
  *(gpio+((gpio_number)/10)) &= ~(7<<(((gpio_number)%10)*3));
}

static inline void pi_set_output(const int gpio_number) {
  // First set to 000 using input function.
  pi_set_input(gpio_number);
  // Next set bit 0 to 1 to set output.
  *(gpio+((gpio_number)/10)) |=  (1<<(((gpio_number)%10)*3));
}

static inline void pi_set_high(const int gpio_number) {
  *(gpio+7) = 1 << gpio_number;
}

static inline void pi_set_low(const int gpio_number) {
  *(gpio+10) = 1 << gpio_number;
}

static inline uint32_t pi_input(const int gpio_number) {
  return *(gpio+13) & (1 << gpio_number);
}
#endif
