//*****************************************************************************
//      Filename : sensor.cpp
// ----------------------------------------------------------------------------
//
// Author:  Lukas Kaim
// Date:    20.12.2008
//
//*****************************************************************************


#include <stdio.h>
#include <windows.h>
#include <iostream.h>
#include <fstream.h>

#include "megsv.h"
#include "Sensor.h"
#include "timer.h"




Sensor::Sensor(float aSensorFactor) { 
    if (GSVactivate(COM_NR, BUFF_SIZE) != GSV_OK) 
    {
        printf("\nSensor-Error: Sensor initialisation failed!\n");
    }
    factorN_int = aSensorFactor;
}




Sensor::~Sensor() { 
    GSVrelease(COM_NR);
}




void Sensor::reset()
{
    printf ("\nSensor calibration running\n");
    printf ("---------------------------\n");

    // setModeLog ----------------------------
    if ( GSVsetModeLog(COM_NR, 1) != GSV_OK )
    {
        Error("Sensor-Error: Logger-mode activation failed!");
    }
    timer.SetZero();
    while(timer.GetMS() < 100) {}


    // setFreq -------------------------------
    if ( GSVsetFreq(COM_NR, FREQ) != GSV_OK )
    {
        Error("Sensor-Error: Frequency adjustment failed!");
    }
    timer.SetZero();
    while(timer.GetMS() < 100) {}

       
    // setGain -------------------------------
    if ( GSVsetGain(COM_NR, GAIN) != GSV_OK )
    {
        Error("Sensor-Error: Gain facto adjustment failed!");
    }
    timer.SetZero();
    while(timer.GetMS() < 100) {}



    // 1 Sekunde abwarten + Ausgabe auf der Konsole
    printf ("\n\rZero point setting");
    timer.SetZero();
    while(timer.GetMS() < 500) {}

    printf ("\rZero point setting .");
    timer.SetZero();
    while(timer.GetMS() < 500) {}


    // setZero -------------------------------
    if ( GSVsetZero(COM_NR) != GSV_TRUE )
    {
        Error("Sensor-Error: Zero point setting failed!");
    }
    

    // 1 Sekunde abwarten
    printf ("\rZero point setting ..");
    timer.SetZero();
    while(timer.GetMS() < 500) {}

    printf ("\rZero point setting ...");
    timer.SetZero();
    while(timer.GetMS() < 500) {}


    printf ("\r1. Place the calibration load on the sensor.\n"); 
    printf ("\n2. Press enter to continue ..."); 
		
	
	while(getchar() != 10) {}   //Bei Enter abbrechen



    // Sensor-Faktor berechnen -----------------------------------------------------
    int valuesNr = 0;
    double value;
    double meanMeasuredSensorValue = 0.0;

    for (int i = 0; i < 100; i++)
    {
        // Befehl an den Sensor senden, damit er daten schickt
        if ( GSVgetValue(COM_NR) == GSV_OK ) 
        {
            // So lange warten bis Daten vom Sensor gekommen sind
            while (GSVread(COM_NR, &value) != GSV_TRUE) {}

            meanMeasuredSensorValue += (float)value;
            valuesNr++;
        }
    }
    meanMeasuredSensorValue /= valuesNr; 
    factorN_int  = 9.80665 / meanMeasuredSensorValue;


    // Sensor-Faktor in Datei speichern --------------------------------------------
    FILE *fp;
    fp = fopen("C:\\CalibPhantom\\ForceSensorCalib.dat", "w");
    fprintf(fp, "%f", (float)factorN_int);
    fclose(fp);


    printf ("\nSensor calibration done.\n");
    printf ("---------------------------\n\n");

    this->checkSensorCalib();
}




void Sensor::printSensorData()
{
    printf ("  Sensor - Frequency: \t\t %f \n",   GSVgetFreq(COM_NR)    );
    printf ("  Sensor - Gain factor: \t %f \n",   GSVgetGain(COM_NR)    );
    printf ("  Sensor - Baudrate: \t %f \n",      GSVgetBaud(COM_NR)    );
    printf ("  Sensor - Logger-mode: \t %f \n\n", GSVgetModeLog(COM_NR) );
}



double Sensor::getValue()
{
    double ad;

    // Übertragung eines Messwertes mit GSVgetValue auslösen
    // Wird bei Logger-Modus benötigt
    if ( GSVgetValue(COM_NR) == GSV_OK ) 
    {
        if ( GSVread(COM_NR, &ad) == GSV_TRUE ) 
        {
            value_int = ad * factorN_int;
            //printf ("  %3.2f\r", value_int);
            return value_int;
        }
        else {
            //cout << "Sensor-Fehler: Keine Messdaten im Puffer!" << endl;
            // Fehlermeldung auskommentiert, da sie beim Programmstart immer ausgegeben wurde
        }
    }
    else {
        printf ("Sensor-Error: Data transmission from sensor disturbed!");
    }
    return value_int;
}



void Sensor::flushBuffer() 
{
    GSVflushBuffer(COM_NR);
}



void Sensor::checkSensorCalib() {
    int valuesNr = 0;
    double value;
    double meanMeasuredSensorValue = 0.0;

    for (int i = 0; i < 100; i++)
    {
        // Befehl an den Sensor senden, damit er daten schickt
        if ( GSVgetValue(COM_NR) == GSV_OK ) 
        {

            // So lange warten bis Daten vom Sensor gekommen sind
            while (GSVread(COM_NR, &value) != GSV_TRUE) {}

            meanMeasuredSensorValue += value * factorN_int;
            valuesNr++;
        }
    }
    meanMeasuredSensorValue /= valuesNr;

    printf("\nMeasured force: %3.2f N\n", (float)meanMeasuredSensorValue);
}



void Sensor::Error(char * msg) {
    printf ("\n %s \n", msg );
    printf ("Press return to continue.");
    while(getchar() != 10) {}   //Bei Enter abbrechen
    exit(0);
}
