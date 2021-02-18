#include "i2c_driver.h"
#include "res.h"
#include "stdint.h"

int main(void) {
  RES res_l;

  res_l = I2C_InitInstance();
  if (res_l != RES_OK) {
    goto exit;
  }

  while (1) {
    res_l = I2C_CheckDevice(0x41u, 200u);
    if (res_l != RES_OK) {
      goto exit;
    }

    res_l = I2C_CheckDevice(0x42u, 200u);
    if (res_l != RES_NOT_FOUND) {
      goto exit;
    }

    {
      uint8_t u8Cmd_l = 0x02u;
      uint8_t u8Data_l = 0x04u;
      res_l = I2C_WriteData(0x41u, 1u, &u8Cmd_l, 1u, &u8Data_l, 600u);
      if (res_l != RES_OK) {
        goto exit;
      }
    }

    {
      uint8_t u8Cmd_l = 0x00u;
      uint8_t au8Data_l[2] = {0x00u, 0x00u};
      res_l = I2C_ReadData(0x41u, 1u, &u8Cmd_l, 2u, au8Data_l, 600u);
      if (res_l != RES_OK) {
        goto exit;
      }

      if ((au8Data_l[0u] != 0x08u) || (au8Data_l[1u] != 0x11u)) {
        res_l = RES_ERROR;
        goto exit;
      }
    }
  }

  res_l = RES_OK;

exit:
  return res_l;
}