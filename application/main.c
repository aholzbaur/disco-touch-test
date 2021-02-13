#include "stm32f4xx_ll_i2c.h"

static int I2C_IsDeviceConnected(uint8_t u8Address) {
  return 0;
}

int main(void) {
  if (I2C_IsDeviceConnected(0xF1u) != 0)
    return -2;

  for (;;)
    ;

  return -1;
}