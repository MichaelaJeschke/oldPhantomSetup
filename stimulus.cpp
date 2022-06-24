//*****************************************************************************
//      Filename : Stimulus.cpp
// ----------------------------------------------------------------------------
// Partly Illusionary EDGE bound by Workspace derived from gstForceField.
//
// SensAble Technologies, Inc. Copyright © 1999
// All rights reserved.
// Author: Lukas Kaim 2008
//*****************************************************************************

#include "stimulus.h"



gstType	Stimulus::StimulusClassTypeId;



Stimulus::Stimulus(Sensor *aSensor) 
{	
	recStatus_int = false;
	boundByBox(gstPoint(0,0,0), 1000.0, 1000.0, 1000.0);
    sensor_int = aSensor;
};




void Stimulus::setStimParams() 
{ 
	for (int i = 0; i < REC_INTERVAL; i++) 
    {
		aveVelocityY_int[i] = 0.0;
	}

	stimOn_int = false;
	integrCount_int = 0;
    currCursPos_int = NO_STIM;
	currCursPosInStim_int = NOT_STIM;
    phantPos_int = gstPoint(0.0, 0.0, 0.0); 
    phantLastPos_int = gstPoint(0.0, 0.0, 0.0);

    phantBackForce_int.dir[0] = 0.0;
    phantBackForce_int.dir[1] = 1.0;
    phantBackForce_int.dir[2] = 0.0;
}



void Stimulus::stimOn (void) { stimOn_int = true; };


void Stimulus::stimOff(void) { stimOn_int = false; };



double Stimulus::getAveVelocityY ()
{
	double ret_vel = 0.0;
    for (int i = 0; i < REC_INTERVAL; i++) 
    {
		ret_vel+=aveVelocityY_int[i];
    }
	return ret_vel / REC_INTERVAL;
}




// gstForceField method overloaded to define the force applied by the
// PHANToM when the  PHANToM has entered the bounding volume associated
// with the force field
// Die Funktion wird 1mal pro ms aufgerufen und gibt in Abhaengigkeit von 
// d. Phantompos. eine Kraft zurueck (als Vektor)
gstVector Stimulus::calculateForceFieldForce (gstPHANToM *aPhantom) 
{    
    
    if (recStatus_int) 
    {
        nowtime_int = timerK_int->GetMS();

        if (nowtime_int >= nexttime_int)
        {
            recPhantData(aPhantom);

            nexttime_int = nowtime_int + REC_INTERVAL;
        }
    }


    phantPos_int = fromWorld(aPhantom->getPosition_WC());


    // Cursor innerhalb des rechten Simulus
    if ( (phantPos_int[0] <  ((STIM_RADIUS*2)) + STIM_BETWEEN/2.0) &&  // kleiner als x-pos (rechts)
         (phantPos_int[0] >  ((STIM_BETWEEN/2.0))) &&    // größer als x-neg (links)
         (phantPos_int[1] <  (STIM_HEIGHT)) &&	// right stim: STIM_BETWEEN/2.0 + (STIM_RADIUS-7), STIM_HEIGHT+5.0, -14
         (phantPos_int[2] > (-STIM_RADIUS-12)) &&  //  left stim: -STIM_BETWEEN/2.0 - STIM_RADIUS, STIM_HEIGHT+5.0, -14
         (phantPos_int[2] <  (STIM_RADIUS-12)) && 
         (stimOn_int == true)) 
    {
        //cout << " R " << endl;
        currCursPos_int = RIGHT_STIM;
		
		int x=(phantPos_int[0] -STIM_RADIUS - STIM_BETWEEN/2.0); // InnerStim von Alexandra
		int y=(phantPos_int[2]+12);
		int r_sq=(x * x + y * y);
		double sq_root=sqrt((double) r_sq);
		if ( (sq_root)> STIM_INNERRADIUS)
		{ currCursPosInStim_int = OUTER_STIM;
		}
		else {
			currCursPosInStim_int = INNER_STIM;
		}
    }


    // Cursor innerhalb des linken Simulus
    else if ( (phantPos_int[0] < (-STIM_BETWEEN/2.0)) &&  // kleiner als x-pos (rechts)
              (phantPos_int[0] > (-STIM_RADIUS*2 - STIM_BETWEEN/2.0)) &&  // größer als x-neg (links)
              (phantPos_int[1] <  (STIM_HEIGHT)) &&
              (phantPos_int[2] > (-STIM_RADIUS-12)) && 
              (phantPos_int[2] <  (STIM_RADIUS-12)) && 
              (stimOn_int == true) )
    {
        //cout << " L " << endl;
        currCursPos_int = LEFT_STIM;

		int x=(phantPos_int[0] +(STIM_RADIUS-4) + STIM_BETWEEN/2.0);
		int y=(phantPos_int[2]+12);
		int r_sq=(x * x + y * y);
		double sq_root=sqrt((double) r_sq);
		if ( (sq_root)> STIM_INNERRADIUS)
		{ currCursPosInStim_int = OUTER_STIM;
		}
		else {
			currCursPosInStim_int = INNER_STIM;
		}
    }


    else {
        //cout << " no stim " << endl;
        currCursPos_int = NO_STIM; 
		currCursPosInStim_int = NOT_STIM;
    }


    if (stimOn_int) 
    {
        aveVelocityY_int[integrCount_int] = phantLastPos_int[1] - phantPos_int[1];
        integrCount_int++;
        integrCount_int %= REC_INTERVAL;
    }

    phantLastPos_int = phantPos_int;

    phantBackForce_int.size = -phantPos_int[1];

    if (phantBackForce_int.size <= 0) {
        phantBackForce_int.size = 0.0;
    }

    phantBackForce_int.prod = phantBackForce_int.size * PLAIN_STIFF * phantBackForce_int.dir;

    return phantBackForce_int.prod;
}





gstVector Stimulus::calculateForceFieldForce (gstPHANToM *aPhantom,
                                                gstVector &aTorque) 
{
    aTorque = gstVector(0, 0, 0);
    return calculateForceFieldForce(aPhantom);
}


	

void Stimulus::startRecPhantData () 
{     
    // Das erste Elem. d. verketteten Liste erzeugen.
    firstRecPhantData_global = new RecordedPhantomData();
    
    // Es existiert an dieser Stelle nur ein Listenelement. Alle Zeiger
    // zeigen auf dieses Element
    nextRecPhantData_global = firstRecPhantData_global;

    timerK_int = new TimerK();

    nexttime_int = REC_INTERVAL;

    recStatus_int = true;  // Das record-flag setzen. Die Phantom-Angaben werden nur
                           // dann gespeichert, wenn recStatus_int == true ist.
};



// Stores the phantom data in a list.
// Is called ones per ms. 
void Stimulus::recPhantData(gstPHANToM *aPhantom) 
{ 
    RecordedPhantomData *tempRecPhantData; 

    // Beim ersten Aufruf von recPhantData verweist nextRecPhantData_global 
    // auf das erste Elem. der verkettetetn Liste. Dieses Elem. wird in 
    // in startRecPhantData erzeugt wurde.
	tempRecPhantData = nextRecPhantData_global;

	nextRecPhantData_global = new RecordedPhantomData();

	tempRecPhantData->next  = nextRecPhantData_global;

    tempRecPhantData->Time  = nowtime_int;

    tempRecPhantData->pointPos = fromWorld(aPhantom->getPosition_WC());

    tempRecPhantData->sensorForce = sensor_int->getValue();

    tempRecPhantData->aveVelocityY = getAveVelocityY();

    tempRecPhantData->whichStimValues = currCursPos_int;

	tempRecPhantData->whichStimArea = currCursPosInStim_int;

    if (!nextRecPhantData_global) {
        cout << " Allocation Error !!! " << endl; 
        exit(0);
    }
}



// Speichert die Phantom-Daten in einer .trj Datei und loescht diese Daten
// gleichzeitig aus dem Speicher (loescht die verkettete Liste)
void Stimulus::storePhantomData(char* aFileName) 
{ 
    recStatus_int = false;
    ofstream fileOutputStream(aFileName);

    // tempRecPhantData zeigt auf das 1. Elem der Liste mit den 
    // Phantom-Werten
    RecordedPhantomData *tempRecPhantData = firstRecPhantData_global;


    // If the trial was OK than store all elements from the list in a file
    while (!(tempRecPhantData == nextRecPhantData_global)) {

        // Output zum File
        fileOutputStream << tempRecPhantData->Time            << " " << 
                            tempRecPhantData->pointPos        << " " << 
                            tempRecPhantData->sensorForce     << " " << 
                            tempRecPhantData->aveVelocityY    << " " << 
                            tempRecPhantData->whichStimValues << " " <<
							tempRecPhantData->whichStimArea<< endl;

        firstRecPhantData_global = tempRecPhantData;

        tempRecPhantData  = firstRecPhantData_global -> next;
        
        delete firstRecPhantData_global;
    }

    fileOutputStream.close();
    delete tempRecPhantData;
    delete timerK_int;  
}



int Stimulus::getCurrCursorPos(void) 
{
    return currCursPos_int;
}

int Stimulus::getCurrCursorPosInStim(void) 
{
    return currCursPosInStim_int;
}


void Stimulus::display (int aStimNr) 
{    
    glPushMatrix();
        if (aStimNr == RIGHT_STIM)
            glTranslatef(0, 0, 25);

        else if (aStimNr == LEFT_STIM)
            glTranslatef(0, 0, -25); 

        glBegin(GL_QUADS);
            glVertex3f(-50, 1, -20);
            glVertex3f(-50, 1,  20);
            glVertex3f( 50, 1,  20);
            glVertex3f( 50, 1, -20);
        glEnd();
    glPopMatrix();
}
