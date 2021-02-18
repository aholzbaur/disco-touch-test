#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include "stdint.h"
#include "res.h"

RES I2C_InitInstance();
RES I2C_CheckDevice(uint8_t u8Addr_p, uint32_t u32Timeout_p);
RES I2C_WriteData(uint8_t u8Addr_p, uint8_t u8NoOfCmds_p, uint8_t *pu8Cmds_p, uint8_t u8NoOfData_p, uint8_t *pu8Data_p, uint32_t u32Timeout_p);
RES I2C_ReadData(uint8_t u8Addr_p, uint8_t u8NoOfCmds_p, uint8_t *pu8Cmds_p, uint8_t u8NoOfData_p, uint8_t *pu8Data_p, uint32_t u32Timeout_p);

#endif