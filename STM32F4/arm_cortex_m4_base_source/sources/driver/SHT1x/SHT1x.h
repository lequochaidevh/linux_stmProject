/**
 * SHT1x Library
 *
 * Copyright 2009 Jonathan Oxer <jon@oxer.com.au> / <www.practicalarduino.com>
 * Based on previous work by:
 *    Maurice Ribble: <www.glacialwanderer.com/hobbyrobotics/?p=5>
 *    Wayne ?: <ragingreality.blogspot.com/2008/01/ardunio-and-sht15.html>
 *
 * Manages communication with SHT1x series (SHT10, SHT11, SHT15)
 * temperature / humidity sensors from Sensirion (www.sensirion.com).
 */
#ifndef SHT1x_h
#define SHT1x_h

#if (ARDUINO >= 100)
#include <Arduino.h>
#elif defined (STM32L_PLATFORM)
#include "../platform/stm32l/arduino/Arduino.h"
#include "../platform/stm32l/io_cfg.h"
#elif defined (STM32F40XX)
#include "../platform/stm32f4/arduino/Arduino.h"
#include "../platform/stm32f4/io_cfg.h"
#else
#include <WProgram.h>
#endif

class SHT1x
{
public:
    SHT1x();
    float readHumidity();
    float readTemperatureC();
    float readTemperatureF();

private:
    int _dataPin;
    int _clockPin;
    int _numBits;
    float readTemperatureRaw();
    int shiftIn(int _dataPin, int _clockPin, int _numBits);
    void sendCommandSHT(int _command, int _dataPin, int _clockPin);
    void waitForResultSHT(int _dataPin);
    int getData16SHT(int _dataPin, int _clockPin);
    void skipCrcSHT(int _dataPin, int _clockPin);
};

#endif
