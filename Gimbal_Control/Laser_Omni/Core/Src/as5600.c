//
// Created by WorriedBottle on 2025/6/18.
//
#include "as5600.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"


uint16_t _rawStartAngle=0;
uint16_t _zPosition=0;
uint16_t _rawEndAngle=0;
uint16_t _mPosition=0;
uint16_t _maxAngle=0;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t Hardware_I2C_Read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Read(&hi2c1, dev_addr << 1, reg_addr,
                              I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);

    return (status == HAL_OK) ? 0 : 1;
}

uint8_t Hardware_I2C_Write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Write(&hi2c1, dev_addr << 1, reg_addr,
                               I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);

    return (status == HAL_OK) ? 0 : 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t highByte(uint16_t value)
{
    uint8_t ret;
    value = value>>8;
    ret = (uint8_t)value;
    return ret;
}

uint8_t lowByte(uint16_t value)
{
    uint8_t ret;
    value = value&0x00ff;
    ret = (uint8_t)value;
    return ret;
}

/*******************************************************
 Method: readOneByte
 In: register to read
 Out: data read from i2c
 Description: reads one byte register from i2c
******************************************************/
uint8_t readOneByte(uint8_t in_adr)
{
    uint8_t retVal = -1;
    Hardware_I2C_Read(_ams5600_Address, in_adr, &retVal, 1);
    return retVal;
}

/*******************************************************
 Method: readOnTwoByte
 In: two registers to read
 Out: data read from i2c as a int16_t
 Description: reads two bytes register from i2c
******************************************************/
uint16_t readTwoBytes(uint8_t in_adr_hi, uint8_t in_adr_lo)
{
    uint16_t retVal = -1;
    uint8_t low=0,high=0;

    /* Read Low Byte */
    low = readOneByte(in_adr_lo);


    /* Read High Byte */
    high = readOneByte(in_adr_hi);

    //printf("high:%d,low:%d  ",high,low);
    retVal = high << 8;
    retVal = retVal | low;
    //printf("retVal:%d\r\n",retVal);
    return retVal;
}

/*******************************************************
 Method: writeOneByte
 In: address and data to write
 Out: none
 Description: writes one byte to a i2c register
******************************************************/
void writeOneByte(uint8_t adr_in, uint8_t dat_in)
{
    Hardware_I2C_Write(_ams5600_Address, adr_in, &dat_in, 1);
}

/****************************************************
 Method: AMS_5600
 In: none
 Out: i2c address of AMS 5600
 Description: returns i2c address of AMS 5600
   **************************************************/
int16_t getAddress()
{
    return _ams5600_Address;
}

/*******************************************************
 Method: getMaxAngle
 In: none
 Out: value of max angle register
 Description: gets value of maximum angle register.
******************************************************/
int16_t getMaxAngle(void)
{
    return readTwoBytes(_mang_hi, _mang_lo);
}

/*******************************************************
 Method: getRawAngle
 In: none
 Out: value of raw angle register
 Description: gets raw value of magnet position.
 start, end, and max angle settings do not apply
******************************************************/
int16_t getRawAngle(void)
{
    return readTwoBytes(_raw_ang_hi, _raw_ang_lo);
}

/*******************************************************
 Method: getStartPosition
 In: none
 Out: value of start position register
 Description: gets value of start position register.
******************************************************/
int16_t getStartPosition(void)
{
    return readTwoBytes(_zpos_hi, _zpos_lo);
}

/*******************************************************
 Method: getEndPosition
 In: none
 Out: value of end position register
 Description: gets value of end position register.
******************************************************/
int16_t getEndPosition(void)
{
    int16_t retVal = readTwoBytes(_mpos_hi, _mpos_lo);
    return retVal;
}

/*******************************************************
 Method: getScaledAngle
 In: none
 Out: value of scaled angle register
 Description: gets scaled value of magnet position.
 start, end, or max angle settings are used to
 determine value
******************************************************/
int16_t getScaledAngle(void)
{
    return readTwoBytes(_ang_hi, _ang_lo);
}

/*******************************************************
 Method: get Agc
 In: none
 Out: value of AGC register
 Description: gets value of AGC register.
******************************************************/
int16_t getAgc()
{
    return readOneByte(_agc);
}

/*******************************************************
 Method: getMagnitude
 In: none
 Out: value of magnitude register
 Description: gets value of magnitude register.
******************************************************/
int16_t getMagnitude()
{
    return readTwoBytes(_mag_hi, _mag_lo);
}

/*******************************************************
 Method: getBurnCount
 In: none
 Out: value of zmco register
 Description: determines how many times chip has been
 permanently written to.
******************************************************/
int16_t getBurnCount()
{
    return readOneByte(_zmco);
}
/*******************************************************
 Method: getRawAngle
 In: none
 Out: value of raw angle register
 Description: gets raw value of magnet position.
 start, end, and max angle settings do not apply
******************************************************/
int16_t AgetRawAngle(void)
{
    return readTwoBytes(_raw_ang_hi, _raw_ang_lo);
}

/*******************************************************
 Method: setEndtPosition
 In: new end angle position
 Out: value of end position register
 Description: sets a value in end position register.
 If no value is provided, method will read position of
 magnet.
******************************************************/
int16_t setEndPosition(int16_t endAngle)
{
    if(endAngle == -1)
        _rawEndAngle = getRawAngle();
    else
        _rawEndAngle = endAngle;

    writeOneByte(_mpos_hi, highByte(_rawEndAngle));
    HAL_Delay(2);
    writeOneByte(_mpos_lo, lowByte(_rawEndAngle));
    HAL_Delay(2);
    _mPosition = readTwoBytes(_mpos_hi, _mpos_lo);

    return(_mPosition);
}

/*******************************************************
 Method: setStartPosition
 In: new start angle position
 Out: value of start position register
 Description: sets a value in start position register.
 If no value is provided, method will read position of
 magnet.
******************************************************/
int16_t setStartPosition(int16_t startAngle)
{
    if(startAngle == -1)
    {
        _rawStartAngle = getRawAngle();
    }
    else
        _rawStartAngle = startAngle;

    writeOneByte(_zpos_hi, highByte(_rawStartAngle));
    HAL_Delay(2);
    writeOneByte(_zpos_lo, lowByte(_rawStartAngle));
    HAL_Delay(2);
    _zPosition = readTwoBytes(_zpos_hi, _zpos_lo);

    return(_zPosition);
}

/*******************************************************
 Method: setMaxAngle
 In: new maximum angle to set OR none
 Out: value of max angle register
 Description: sets a value in maximum angle register.
 If no value is provided, method will read position of
 magnet.  Setting this register zeros out max position
 register.
******************************************************/
int16_t setMaxAngle(int16_t newMaxAngle)
{
    int32_t retVal;
    if(newMaxAngle == -1)
    {
        _maxAngle = getRawAngle();
    }
    else
        _maxAngle = newMaxAngle;

    writeOneByte(_mang_hi, highByte(_maxAngle));
    HAL_Delay(2);
    writeOneByte(_mang_lo, lowByte(_maxAngle));
    HAL_Delay(2);

    retVal = readTwoBytes(_mang_hi, _mang_lo);
    return retVal;
}

/*******************************************************
 Method: detectMagnet
 In: none
 Out: 1 if magnet is detected, 0 if not
 Description: reads status register and examines the
 MH bit
******************************************************/
uint8_t detectMagnet(void)
{
    uint8_t magStatus;
    uint8_t retVal = 0;
    /*0 0 MD ML MH 0 0 0*/
    /* MD high = AGC minimum overflow, Magnet to strong */
    /* ML high = AGC Maximum overflow, magnet to weak*/
    /* MH high = magnet detected*/
    magStatus = readOneByte(_stat);

    if(magStatus & 0x20)
        retVal = 1;

    return retVal;
}

/*******************************************************
 Method: getMagnetStrength
 In: none
 Out: 0 if no magnet is detected
      1 if magnet is to weak
      2 if magnet is just right
      3 if magnet is to strong
 Description: reads status register andexamins the MH,ML,MD bits
******************************************************/
uint8_t getMagnetStrength(void)
{
    uint8_t magStatus;
    uint8_t retVal = 0;
    /*0 0 MD ML MH 0 0 0*/
    /* MD high = AGC minimum overflow, Magnet to strong */
    /* ML high = AGC Maximum overflow, magnet to weak*/
    /* MH high = magnet detected*/
    magStatus = readOneByte(_stat);
    if(detectMagnet() ==1)
    {
        retVal = 2; /*just right */
        if(magStatus & 0x10)
            retVal = 1; /*to weak */
        else if(magStatus & 0x08)
            retVal = 3; /*to strong */
    }

    return retVal;
}

/*******************************************************
 Method: burnAngle
 In: none
 Out: 1 success
     -1 no magnet
     -2 burn limit exceeded
     -3 start and end positions not set (useless burn)
 Description: burns start and end positions to chip.
 THIS CAN ONLY BE DONE 3 TIMES
******************************************************/
uint8_t burnAngle()
{
    uint8_t retVal = 1;
    _zPosition = getStartPosition();
    _mPosition = getEndPosition();
    _maxAngle  = getMaxAngle();

    if(detectMagnet() == 1)
    {
        if(getBurnCount() < 3)
        {
            if((_zPosition == 0)&&(_mPosition ==0))
                retVal = -3;
            else
                writeOneByte(_burn, 0x80);
        }
        else
            retVal = -2;
    }
    else
        retVal = -1;

    return retVal;
}

/*******************************************************
 Method: burnMaxAngleAndConfig
 In: none
 Out: 1 success
     -1 burn limit exceeded
     -2 max angle is to small, must be at or above 18 degrees
 Description: burns max angle and config data to chip.
 THIS CAN ONLY BE DONE 1 TIME
******************************************************/
uint8_t burnMaxAngleAndConfig()
{
    uint8_t retVal = 1;
    _maxAngle  = getMaxAngle();

    if(getBurnCount() ==0)
    {
        if(_maxAngle*0.087 < 18)
            retVal = -2;
        else
            writeOneByte(_burn, 0x40);
    }
    else
        retVal = -1;

    return retVal;
}

/**
  * @brief  ´®¿ÚÊäÈë³ÌÐò
  * @param
  * @note
  * @retval ·µ»ØµÚÒ»¸ö½ÓÊÕµÄ×Ö½Ú
  * @author ÖÇÓùµç×Ó
  */
uint8_t PrintInFromConsole(void)
{
    uint8_t data;
    //HAL_UART_Receive(&huart1, &data, 1, 0xFFFF);
    return data;
}

/**
  * @brief  ´®¿Ú½»»¥½çÃæ
  * @param
  * @note
  * @retval void
  * @author ÖÇÓùµç×Ó
  */
int8_t  MenuChoice=0;
void PrintMenu(void)
{
//    printf("AS5600 Serial Interface Programe----SC-electronic\r\n");
//    printf("1 - Set start position\t|  "); printf(" 6 - get MPOS\r\n");
//    printf("2 - Set end position\t|  ");   printf(" 7 - get raw angle\r\n");
//    printf("3 - Set max angle range\t|  ");  printf(" 8 - get scaled angle\r\n");
//    printf("4 - Get max angle range\t|  ");  printf(" 9 - detect magnet\r\n");
//    printf("5 - Get ZPOS \t\t|  ");     printf("10 - get magnet strength\r\n");
//    printf("\r\n");
//    printf("Number of burns remaining: "); printf("%d\r\n",(3 - getBurnCount()));
//    printf("96 - Burn Angle\r\n");
//    printf("98 - Burn Settings (one time)\r\n");
    MenuChoice = PrintInFromConsole();
//    printf("You choice NO.%d programe\r\n",MenuChoice);
}

/*******************************************************
 Function: convertRawAngleToDegrees
 In: angle data from AMS_5600::getRawAngle
 Out: human readable degrees as float
 Description: takes the raw angle and calculates
 float value in degrees.
******************************************************/
float convertRawAngleToDegrees(int16_t newAngle)
{
    /* Raw data reports 0 - 4095 segments, which is 0.087 of a degree */
    float retVal = newAngle * 0.087;
    return retVal;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t rawdata = 0;
float degrees = 0;
char message[50];
void Programe_Run(void)
{
    uint8_t dect= 0;
    dect = detectMagnet();
    sprintf(message, "detectMagnet is %d\r\n", dect);
    HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
//    printf("detectMagnet is %d\r\n",dect);
    rawdata = getRawAngle();
    sprintf(message, "rawdata is %d\r\n", rawdata);
    HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
//    printf("rawdata is %d\r\n",rawdata);
    degrees = convertRawAngleToDegrees(rawdata);
    sprintf(message, "degrees is %f\r\n",degrees);
    HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
//    printf("degrees is %f\r\n",degrees);
}