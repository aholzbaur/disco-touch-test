#include "i2c_driver.h"

#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_i2c.h"

static RES I2C_WriteCmds(uint8_t u8Addr_p, uint8_t u8NoOfCmds_p, uint8_t *pu8Cmds_p, uint32_t u32Timeout_p, uint32_t *pu32Time_p) {
  LL_I2C_GenerateStartCondition(I2C3);

  while (LL_I2C_IsActiveFlag_SB(I2C3) == 0u) {
    if ((*pu32Time_p)++ >= u32Timeout_p) {
      return RES_ERROR;
    }
  }

  LL_I2C_AcknowledgeNextData(I2C3, LL_I2C_ACK);

  LL_I2C_TransmitData8(I2C3, (u8Addr_p << 1u) & 0xFEu);

  while (LL_I2C_IsActiveFlag_ADDR(I2C3) == 0u) {
    if ((*pu32Time_p)++ >= u32Timeout_p) {
      return RES_NOT_FOUND;
    }
  }

  LL_I2C_ClearFlag_ADDR(I2C3);

  if (u8NoOfCmds_p > 0u) {
    {
      uint8_t u8CmdsSent_l = 0u;
      while (u8CmdsSent_l < u8NoOfCmds_p) {
        while (LL_I2C_IsActiveFlag_TXE(I2C3) == 0u) {
          if ((*pu32Time_p)++ >= u32Timeout_p) {
            return RES_ERROR;
          }
        }

        LL_I2C_TransmitData8(I2C3, pu8Cmds_p[u8CmdsSent_l++]);
      }
    }

    while (LL_I2C_IsActiveFlag_TXE(I2C3) == 0u) {
      if ((*pu32Time_p)++ >= u32Timeout_p) {
        return RES_ERROR;
      }
    }
  }

  return RES_OK;
}

RES I2C_ReadData(uint8_t u8Addr_p, uint8_t u8NoOfCmds_p, uint8_t *pu8Cmds_p, uint8_t u8NoOfData_p, uint8_t *pu8Data_p, uint32_t u32Timeout_p) {
  uint32_t u32Time_l = 0u;

  {
    RES eResult_l = I2C_WriteCmds(u8Addr_p, u8NoOfCmds_p, pu8Cmds_p, u32Timeout_p, &u32Time_l);
    if (eResult_l != RES_OK) {
      LL_I2C_GenerateStopCondition(I2C3);
      return eResult_l;
    }
  }

  LL_I2C_GenerateStartCondition(I2C3);

  while (LL_I2C_IsActiveFlag_SB(I2C3) == 0u) {
    if (u32Time_l++ >= u32Timeout_p) {
      LL_I2C_GenerateStopCondition(I2C3);
      return RES_ERROR;
    }
  }

  LL_I2C_AcknowledgeNextData(I2C3, LL_I2C_ACK);

  LL_I2C_TransmitData8(I2C3, (u8Addr_p << 1u) | 0x01u);

  while (LL_I2C_IsActiveFlag_ADDR(I2C3) == 0u) {
    if (u32Time_l++ >= u32Timeout_p) {
      LL_I2C_GenerateStopCondition(I2C3);
      return RES_NOT_FOUND;
    }
  }

  if (u8NoOfData_p == 1u) {
    LL_I2C_AcknowledgeNextData(I2C3, LL_I2C_NACK);
  }
  LL_I2C_ClearFlag_ADDR(I2C3);

  {
    uint8_t u8DataRead_l = 0u;
    while (u8DataRead_l < u8NoOfData_p) {
      if (u8DataRead_l + 2u > u8NoOfData_p) {
        LL_I2C_AcknowledgeNextData(I2C3, LL_I2C_NACK);
        LL_I2C_GenerateStopCondition(I2C3);
      }

      while (LL_I2C_IsActiveFlag_RXNE(I2C3) == 0u) {
        if (u32Time_l++ >= u32Timeout_p) {
          LL_I2C_GenerateStopCondition(I2C3);
          return RES_ERROR;
        }
      }

      pu8Data_p[u8DataRead_l++] = LL_I2C_ReceiveData8(I2C3);
    }
  }

  return RES_OK;
}

RES I2C_WriteData(uint8_t u8Addr_p, uint8_t u8NoOfCmds_p, uint8_t *pu8Cmds_p, uint8_t u8NoOfData_p, uint8_t *pu8Data_p, uint32_t u32Timeout_p) {
  uint32_t u32Time_l = 0u;

  {
    RES eResult_l = I2C_WriteCmds(u8Addr_p, u8NoOfCmds_p, pu8Cmds_p, u32Timeout_p, &u32Time_l);
    if (eResult_l != RES_OK) {
      LL_I2C_GenerateStopCondition(I2C3);
      return eResult_l;
    }
  }

  if (u8NoOfData_p > 0u) {
    {
      uint8_t u8DataSent_l = 0u;
      while (u8DataSent_l < u8NoOfData_p) {
        while (LL_I2C_IsActiveFlag_TXE(I2C3) == 0u) {
          if (u32Time_l++ >= u32Timeout_p) {
            LL_I2C_GenerateStopCondition(I2C3);
            return RES_ERROR;
          }
        }

        LL_I2C_TransmitData8(I2C3, pu8Data_p[u8DataSent_l++]);
      }
    }

    while (LL_I2C_IsActiveFlag_TXE(I2C3) == 0u) {
      if (u32Time_l++ >= u32Timeout_p) {
        LL_I2C_GenerateStopCondition(I2C3);
        return RES_ERROR;
      }
    }
  }

  LL_I2C_GenerateStopCondition(I2C3);

  return RES_OK;
}

RES I2C_CheckDevice(uint8_t u8Addr_p, uint32_t u32Timeout_p) {
  return I2C_WriteData(u8Addr_p, 0u, (uint8_t *)0u, 0u, (uint8_t *)0u, u32Timeout_p);
}

RES I2C_InitInstance() {
  LL_GPIO_InitTypeDef gpioInitStruct;
  LL_I2C_InitTypeDef i2cInitStruct;

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

  gpioInitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  gpioInitStruct.Alternate = LL_GPIO_AF_4;
  gpioInitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  gpioInitStruct.Pull = LL_GPIO_PULL_UP;
  gpioInitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;

  gpioInitStruct.Pin = LL_GPIO_PIN_8;
  if (LL_GPIO_Init(GPIOA, &gpioInitStruct) != SUCCESS)
    return RES_ERROR;

  gpioInitStruct.Pin = LL_GPIO_PIN_9;
  if (LL_GPIO_Init(GPIOC, &gpioInitStruct) != SUCCESS)
    return RES_ERROR;

  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C3);

  i2cInitStruct.ClockSpeed = LL_I2C_MAX_SPEED_FAST;
  i2cInitStruct.DigitalFilter = 0u;
  i2cInitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
  i2cInitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  i2cInitStruct.OwnAddress1 = 0x00u;
  i2cInitStruct.DutyCycle = LL_I2C_DUTYCYCLE_2;
  i2cInitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  i2cInitStruct.TypeAcknowledge = LL_I2C_NACK;

  LL_I2C_Disable(I2C3);

  if (LL_I2C_Init(I2C3, &i2cInitStruct) != SUCCESS)
    return RES_ERROR;

  LL_I2C_Enable(I2C3);

  return RES_OK;
}