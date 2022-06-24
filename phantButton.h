//*****************************************************************************
//      Filename : touch_zone.h
// ----------------------------------------------------------------------------
//
// SensAble Technologies, Inc. Copyright © 1999
// All rights reserved.
// Author: Knut Drewing

//*****************************************************************************

#ifndef _phant_button_
#define _phant_button_

#include <gstForceField.h>

#define ON   1
#define OFF  0



class PhantButton : public gstForceField { 
	public:
		PhantButton (gstVector aPos, double size);
				
		// Get type of this class.  No instance needed.
		static gstType getClassTypeId()
		{
			gstAssignUniqueId(touch_cubeClassTypeId);
		return touch_cubeClassTypeId;
		}

		// Get type of this instance.
		virtual gstType getTypeId() const { return getClassTypeId(); }

		// Returns TRUE if this class is same or derived class of type.
		virtual gstBoolean isOfType(gstType type) const {return staticIsOfType(type);}

		// Returns TRUE if subclass is of type.
		static gstBoolean staticIsOfType(gstType type)
		{
			if (type == getClassTypeId()) return true;
		else return (gstForceField::staticIsOfType(type));
		}
		
		virtual gstVector calculateForceFieldForce(gstPHANToM *aPhantom);
		virtual gstVector calculateForceFieldForce(gstPHANToM *aPhantom,
												gstVector &torque);
		double getTimeInContact(void);
        void display(void);
        int  getStatus(void);
        void setOff(void);

	private:
		static gstType touch_cubeClassTypeId;
  	
		double timeInContact_int; 
        double size_int; 
        double sizeHalf_int; 
	  	DWORD  startTime_int;
        DWORD  timeNow_int; 
	  	bool continuePrevious_int;
        gstVector pos_int;
        int status_int;
	};

#endif // TOUCH_ZONE_INCLUDE
