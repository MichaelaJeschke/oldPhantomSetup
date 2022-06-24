// CalibSetup.h: interface for the CalibSetup class.
//
//////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <string.h>


#ifndef CALIBSETUP
#define CALIBSETUP



class CalibSetup {

public:
    // Phantom
    CalibSetup();
    virtual ~CalibSetup();

    inline float getPhantTransX()  { return phantTransX; }
    inline float getPhantTransY()  { return phantTransY; }
    inline float getPhantTransZ()  { return phantTransZ; }
    inline float getPhantScale()   { return phantScale;  }
    inline float getPhantRotX()    { return phantRotX;   }
    inline float getPhantRotY()    { return phantRotY;   }
    inline float getPhantRotZ()    { return phantRotZ;   }

    // Screen
    inline float getScreenTransX() { return screenTransX; }
    inline float getScreenTransY() { return screenTransY; }
    inline float getScreenTransZ() { return screenTransZ; }
    inline float getScreenRotX()   { return screenRotX;   }
    inline float getScreenRotY()   { return screenRotY;   }
    inline float getScreenRotZ()   { return screenRotZ;   }
    inline float getScreenHeight() { return screenHeight; }
    inline float getScreenWidth()  { return screenWidth;  }

    // Eye
    inline float getEyeR_TransX()      { return eyeR_TransX; }
    inline float getEyeL_TransX()      { return eyeL_TransX; }
    inline float getEyeR_TransY()      { return eyeR_TransY; } 
    inline float getEyeL_TransY()      { return eyeL_TransY; }
    inline float getEyeR_TransZ()      { return eyeR_TransZ; }
    inline float getEyeL_TransZ()      { return eyeL_TransZ; }
    inline float getMiddleEye_TransX() { return middleEye_TransX; }


private:
    // Phantom
    float phantTransX;
    float phantTransY;
    float phantTransZ;
    float phantScale;
    float phantRotX;
    float phantRotY;
    float phantRotZ;

    // Screen
    float screenTransX;
    float screenTransY;
    float screenTransZ;
    float screenRotX;
    float screenRotY;
    float screenRotZ;
    float screenHeight;
    float screenWidth;

    // Eye
    float eyeR_TransX;
    float eyeL_TransX;
    float eyeR_TransY;
    float eyeL_TransY;
    float eyeR_TransZ;
    float eyeL_TransZ;
    float middleEye_TransX;

    void ReadPhantData(char* aFileName);
    void ReadScreenData(char* aFileName);
    void ReadEyeData(char* aFileName);
};

#endif