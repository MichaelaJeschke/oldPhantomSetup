//*****************************************************************************
//      Filename : phantButton.cpp
// ----------------------------------------------------------------------------
//
// Author: Lukas Kaim 2008
//*****************************************************************************

#include <time.h>
#include <gstPHANToM.h>
#include <gstPoint.h>
#include <glut.h>
#include "phantButton.h"



gstType	PhantButton::touch_cubeClassTypeId;


// Constructor.
PhantButton::PhantButton (gstVector aPos, double size) { 
	size_int = size; 
    sizeHalf_int = size_int / 2.0;
	timeInContact_int = 0.0;
	continuePrevious_int = false;
	boundByBox (gstPoint(0.0, 0.0, 0.0), size_int, size_int, size_int);
    pos_int = aPos;
    this->setTranslate(aPos);
}

		

gstVector PhantButton::calculateForceFieldForce(gstPHANToM *aPhantom) {
	
    gstPoint phantPos = fromWorld (aPhantom->getPosition_WC()); 
	
	if (phantPos.x()<sizeHalf_int && phantPos.x()>(-sizeHalf_int) &&
		phantPos.y()<sizeHalf_int && phantPos.y()>(-sizeHalf_int) &&
		phantPos.z()<sizeHalf_int && phantPos.z()>(-sizeHalf_int)) {
		if (!continuePrevious_int) {
			continuePrevious_int = true; 
			timeInContact_int = 0.0;
			startTime_int = GetTickCount();
            status_int = ON;
		}
        else {
			timeNow_int = GetTickCount();
			timeInContact_int = (double)(timeNow_int - startTime_int);
		}
	}
	
    else {
        continuePrevious_int = false; 
        timeInContact_int = 0.0;
    }
				    
	return gstVector(0.0,0.0,0.0);
}
	


void PhantButton::display (void) {
    glPushMatrix(); 
        glTranslatef(pos_int.x(), pos_int.y(), pos_int.z()); 
        glutSolidCube(size_int);
    glPopMatrix();
}

	


gstVector PhantButton::calculateForceFieldForce (gstPHANToM *aPhantom,
						                        gstVector &torque) {
    torque = gstVector(0, 0, 0);
    return calculateForceFieldForce(aPhantom);
}



double PhantButton::getTimeInContact (void) { 
    return timeInContact_int;
}



int PhantButton::getStatus (void) {
    return status_int;   
}


void PhantButton::setOff (void) {
    status_int = OFF; 
    timeInContact_int = 0.0;
    continuePrevious_int = false;
}
