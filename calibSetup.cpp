// File:    calibSetup.cpp
// Author:  Lukas Kaim
// Date:    16.01.2008
//


#include "calibSetup.h"


CalibSetup::CalibSetup () { 
 ReadPhantData  ("C:/CalibPhantom/CalibFiles/phantom_calib.cal");
 ReadScreenData ("C:/CalibPhantom/CalibFiles/screen_calib.cal");
 ReadEyeData    ("C:/CalibPhantom/CalibFiles/screen_calib.cal");
}




CalibSetup::~CalibSetup() {  }




void CalibSetup::ReadPhantData (char* aFileName) { 
    
    FILE *file = NULL;
    char input[64];
    int  valuesNr = 0;
    
    if ( (file = fopen(aFileName, "r")) == NULL) 
    {
        cout << endl;
        cout << "Cannot open calib-data file '"<< aFileName <<"'!" << endl;
        cout << "Press return to continue." << endl;
        while(getchar() != 10) {}   //Bei Enter abbrechen
        exit(0);
    }

    while (fscanf(file, "%s", input) != EOF) 
    {
        if (!strcmp (input, "phant_trans_x")) 
        {
            fscanf(file, "%s", input); 
            phantTransX = (float)atof(input);
            valuesNr++;
        }
        else if (!strcmp (input, "phant_trans_y")) 
        {
            fscanf(file, "%s", input); 
            phantTransY = (float)atof(input);
            valuesNr++;
        }
        else if (!strcmp (input, "phant_trans_z")) 
        {
            fscanf(file, "%s", input); 
            phantTransZ = (float)atof(input);
            valuesNr++;
        }
        else if (!strcmp (input, "phant_scale")) 
        {
            fscanf(file, "%s", input); 
            phantScale = (float)atof(input);
            valuesNr++;
        }
        else if (!strcmp (input, "phant_rot_x")) 
        {
            fscanf(file, "%s", input); 
            phantRotX = (float)atof(input);
            valuesNr++;
        }
        else if (!strcmp (input, "phant_rot_y")) 
        {
            fscanf(file, "%s", input); 
            phantRotY = (float)atof(input);
            valuesNr++;
        }
        else if (!strcmp (input, "phant_rot_z")) 
        {
            fscanf(file, "%s", input); 
            phantRotZ = (float)atof(input);
            valuesNr++;
        }
    }
    fclose(file);

    if (valuesNr != 7) {
        cout << "Error in '" << aFileName <<"' file!" << endl;
        cout << "Press return to continue." << endl;
        while(getchar() != 10) {}   // Bei Enter abbrechen
        exit(0);
    }
}





void CalibSetup::ReadScreenData (char* aFileName) { 
    
    FILE *file = NULL;
    char input[64];
    int  valuesNr = 0;
    
    if ( (file = fopen(aFileName, "r")) == NULL) 
    {
        cout << endl;
        cout << "Cannot open calib-data file '"<< aFileName <<"'!" << endl;
        cout << "Press return to continue." << endl;
        while(getchar() != 10) {}   //Bei Enter abbrechen
        exit(0);
    }

    while (fscanf(file, "%s", input) != EOF) 
    {
        if (!strcmp (input, "screen_trans_x")) 
        {
            fscanf(file, "%s", input); 
            screenTransX = (float)atof(input);
            valuesNr++;
        }
        else if (!strcmp (input, "screen_trans_y")) 
        {
            fscanf(file, "%s", input); 
            screenTransY = (float)atof(input);
            valuesNr++;
        }
        else if (!strcmp (input, "screen_trans_z")) 
        {
            fscanf(file, "%s", input); 
            screenTransZ = (float)atof(input);
            valuesNr++;
        }
        else if (!strcmp (input, "screen_rot_x")) 
        {
            fscanf(file, "%s", input); 
            screenRotX = (float) (atof(input) * 180 / 3.141592);
            valuesNr++;
        }
        else if (!strcmp (input, "screen_rot_y")) 
        {
            fscanf(file, "%s", input); 
            screenRotY = (float) (atof(input) * 180 / 3.141592);
            valuesNr++;
        }
        else if (!strcmp (input, "screen_rot_z")) 
        {
            fscanf(file, "%s", input); 
            screenRotZ = (float) (atof(input) * 180 / 3.141592);
            valuesNr++;
        }
        else if (!strcmp (input, "screen_height")) 
        {
            fscanf(file, "%s", input); 
            screenHeight = (float)atof(input);
            valuesNr++;
        }
        else if (!strcmp (input, "screen_width")) 
        {
            fscanf(file, "%s", input); 
            screenWidth = (float)atof(input);
            valuesNr++;
        }
    }
    fclose(file);

    if (valuesNr != 8) {
        cout << "Error in '" << aFileName <<"' file!" << endl;
        cout << "Press return to continue." << endl;
        while(getchar() != 10) {}   // Bei Enter abbrechen
        exit(0);
    }

    // The calib data set the zero position of the Phantom and the screen
    // to the table. That means, that the 0-position is not in 
    // the native (zero-) position of the phantom. 
    // The following code sets the sero position of the screen to the 
    // zero position of the phantom
    screenTransX += phantTransX;
    screenTransY += phantTransY;
    screenTransZ += phantTransZ;
}





void CalibSetup::ReadEyeData (char* aFileName) { 
    
    FILE *file = NULL;
    char input[64];
    int  valuesNr = 0;
    
    if ( (file = fopen(aFileName, "r")) == NULL) 
    {
        cout << endl;
        cout << "Cannot open calib-data file '"<< aFileName <<"'!" << endl;
        cout << "Press return to continue." << endl;
        while(getchar() != 10) {}   //Bei Enter abbrechen
        exit(0);
    }

    while (fscanf(file, "%s", input) != EOF) 
    {
        if (!strcmp (input, "eyeR_trans_x")) 
        {
            fscanf(file, "%s", input); 
            eyeR_TransX = (float)atof(input);
            valuesNr++;
        }
        else if (!strcmp (input, "eyeL_trans_x")) 
        {
            fscanf(file, "%s", input); 
            eyeL_TransX = (float)atof(input);
            valuesNr++;
        }
        else if (!strcmp (input, "eyeR_trans_y")) 
        {
            fscanf(file, "%s", input); 
            eyeR_TransY = (float)atof(input);
            valuesNr++;
        }
        else if (!strcmp (input, "eyeL_trans_y")) 
        {
            fscanf(file, "%s", input); 
            eyeL_TransY = (float)atof(input);
            valuesNr++;
        }
        else if (!strcmp (input, "eyeR_trans_z")) 
        {
            fscanf(file, "%s", input); 
            eyeR_TransZ = (float)atof(input);
            valuesNr++;
        }
        else if (!strcmp (input, "eyeL_trans_z")) 
        {
            fscanf(file, "%s", input); 
            eyeL_TransZ = (float)atof(input);
            valuesNr++;
        }
    }
    fclose(file);


    if (valuesNr != 6) {
        cout << "Error in '" << aFileName <<"' file!" << endl;
        cout << "Press return to continue." << endl;
        while(getchar() != 10) {}   // Bei Enter abbrechen
        exit(0);
    }

    middleEye_TransX = (eyeR_TransX + eyeL_TransX) / 2;
}
