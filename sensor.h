//*****************************************************************************
//      Filename : sensor.h
// ----------------------------------------------------------------------------
//
// Author:  Lukas Kaim
// Date:    20.12.2008
//
//*****************************************************************************


#include "timer.h"
#ifndef _sensor_h_
#define _sensor_h_


#define OK           1
#define COM_NR       1      // Serial port number
#define FREQ       700
#define GAIN         2
#define BUFF_SIZE 1000      // Buffer size for the AD-converter data 


class Sensor {

private:
    double factorN_int;
    double value_int;
    TimerK timer;
    
public:
    Sensor(float aSensorFactor);
    ~Sensor();
    int activate();
    void reset();
    void flushBuffer();
    void printSensorData();
    void checkSensorCalib();
    double getValue();
    void Error(char * msg);
};

#endif