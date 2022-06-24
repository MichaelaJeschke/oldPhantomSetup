//*****************************************************************************
//      Filename : Stimulus.h
// ----------------------------------------------------------------------------
//
// SensAble Technologies, Inc. Copyright © 1999
// All rights reserved.
// Author: Knut Drewing

//*****************************************************************************

#ifndef ILLUS_CURVE_INCLUDE
#define ILLUS_CURVE_INCLUDE

#include <gstPHANToM.h>
#include <gstForceField.h>
#include <glut.h>
#include <math.h>
#include <fstream.h>
#include "timer.h"
#include "dataManager.h"
#include "sound.h"
#include "sensor.h"




// Methods - parameter kept constant?
#define PI              3.14159

#define REC_INTERVAL   3   // record intervals for movement in ms

#define PLAIN_STIFF    1.0

// Angaben zum STIMULUS (d.h. die beiden Huppel)
#define STIM_HEIGHT         13.5//45.0//30.0
#define STIM_BETWEEN        22.0 //18.0-(49.5-41) // statt 10
#define STIM_RADIUS         60.0 // halbe diagonale bei 7x7 quadrat5 //41 for disc  
#define STIM_INNERRADIUS	45.5 // Neu von Textur Scheibe Run (Alex)

#define STIM_X_POS      3.0
#define STIM_Y_POS     -25.0 //-8.0
#define STIM_Z_POS      8.0 // eigetlich x


class Force {
public:
	double size;
	gstVector dir;
	gstVector prod;
};



class RecordedPhantomData {
public:
	long int Time;
	gstPoint pointPos; 
	double sensorForce;
	double aveVelocityY; 
	RecordedPhantomData * next;
    int whichStimValues;
	int whichStimArea; // Alex Texture
};


class Stimulus : public gstForceField {

public:
	
    // Constructor.
    Stimulus(Sensor*); 

    // gstForceField method overloaded to define the force applied by
    // the PHANToM when the  PHANToM has entered the bounding volume
    // associated with the force field
    virtual gstVector calculateForceFieldForce(gstPHANToM *aPhantom);
    virtual gstVector calculateForceFieldForce(gstPHANToM *aPhantom,
                                               gstVector &torque);

	void startRecPhantData(void);
	void storePhantomData(char* aFileName);
	void stimOn(void);
	void stimOff(void);
    void setStimParams(void);
	double getAveVelocityY(void);
    int getCurrCursorPos(void);
	int getCurrCursorPosInStim(void); // Alex Texture
    void display(int);
    Sensor *sensor_int;

private:
	static gstType StimulusClassTypeId;

	double aveVelocityY_int[REC_INTERVAL]; 

    double stimPairWeight_int; 

	int integrCount_int;

	void recPhantData(gstPHANToM *aPhantom);

    // Zeiger auf Elemente einer verketteten Liste in der in einer
    // bestimmten Frequenz (z.B alle 5 ms) die Werte des Phantom 
    // wie Position, Kraft, ... gespeichert werden.
	RecordedPhantomData * firstRecPhantData_global; // Zeiger auf des 1. Listenelement 
	RecordedPhantomData * nextRecPhantData_global;

	TimerK *timerK_int; 

    long int nowtime_int;
    long int nexttime_int; 
    int currCursPos_int;
	int currCursPosInStim_int; // New Alex Texture
	bool stimOn_int;
    bool recStatus_int;

    Force phantBackForce_int;

    gstPoint phantPos_int, 
             phantLastPos_int;
};


#endif // ILLUS_CURVE_INCLUDE
