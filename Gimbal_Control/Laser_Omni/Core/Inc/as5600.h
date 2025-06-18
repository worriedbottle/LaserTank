//
// Created by WorriedBottle on 2025/6/18.
//

#ifndef LASER_OMNI_AS5600_H
#define LASER_OMNI_AS5600_H

#include "i2c.h"

#define AS5600_ADDRESS_MAG           0x36

typedef enum
{
/* set i2c address */
    _ams5600_Address = 0x36,
    _zmco = 0x00,
    _zpos_hi = 0x01,
    _zpos_lo = 0x02,
    _mpos_hi = 0x03,
    _mpos_lo = 0x04,
    _mang_hi = 0x05,
    _mang_lo = 0x06,
    _conf_hi = 0x07,
    _conf_lo = 0x08,
    _raw_ang_hi = 0x0c,
    _raw_ang_lo = 0x0d,
    _ang_hi = 0x0e,
    _ang_lo = 0x0f,
    _stat = 0x0b,
    _agc = 0x1a,
    _mag_hi = 0x1b,
    _mag_lo = 0x1c,
    _burn = 0xff
}AMS5600Registers_t;

void PrintMenu(void);
void Programe_Run(void);

#endif //LASER_OMNI_AS5600_H
